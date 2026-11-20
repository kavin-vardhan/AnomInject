Independent m53 design review — 082-02 — 2026-09-26

The plan requires revision before S1. Route B is a plausible approach for a defined, qualified subset of host textures and materials; the proposed identity test does not establish its general fidelity. There are concrete paths to an ineffective override, an extra visual change, and an inexact restore. Route A is not an acceptable automatic fallback under the stated label-purity requirement.

Chat review REQUIRED for accepting the revised plan and authorizing S1: the D1 residency/fallback and D7 MID rulings need reconsideration. This review does not authorize implementation, builds, cooks, launches, merge, or default-on operation.

**Scope and evidence**

Reviewed the complete plan and journal at 946c1bfecfe30860c622f7ee7c6e463b6fd2fd08, the complete Code report and Chat D1–D7 ruling, the review request, current continuity/operating instructions, relevant plugin source at master 4283fc89cb61c6eb6ffb36a2402d9c4a16da6164, and the local UE 5.1.1 engine implementation. The checkout remained m51 at 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67. Both the local and origin m53 branch refs resolve to the reviewed commit.

Source notation below is explicit because the plan is not checked out:

- PLAN:L = Git blob 946c1bf:docs/predictions/2026-09-26-m53-uv-normal-corruption.md, logical absolute path D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\docs\predictions\2026-09-26-m53-uv-normal-corruption.md, line L.
- PLUGIN/path:L = a file under D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector at 4283fc8, line L, read from Git objects.
- UE/path:L = a local file under D:\UESource\UnrealEngine\Engine, line L.
- RULING:L = D:\IntrusiveAnomalies\_reviews\082-02-chat-ruling-m53-plan-decisions.md, line L.

The findings are source-backed design findings, not observed game failures. In-memory numerical probes checked blend recurrence, mip-chain byte counts, the inactive-parameter alias, and an authored-lower-mip counterexample (identical mip 0, different mip 1). No repository files were changed; the only output written is this requested review. No build, cook, editor/game launch, or capture was performed. No office content was transferred.

**P1-1 — Texture-pointer intersection does not establish an active parameter binding.**

Location: PLAN:128–137, 212–229, 277–282.

Failure: a material's active static-switch branch samples texture T through a non-parameter node. Its inactive branch contains texture parameter P, also defaulting to T. GetUsedTextures returns T; GetAllTextureParameterInfo/GetTextureParameterValue returns P → T. Their proposed pointer intersection accepts P, but overriding P changes nothing. Installing our MID can still satisfy an ownership-based condition_held predicate. This is a structural no-op, separate from a legitimate corruption that happens to be visually weak. Requesting all quality levels at PLAN:227 also admits bindings used only by another loaded quality permutation.

Engine evidence: UE/Source/Runtime/Engine/Private/Materials/MaterialCachedData.cpp:263–306 caches parameter metadata from expressions; Material.cpp:1077–1122 traverses the requested quality range and returns unique texture objects; MaterialInstance.cpp:1012–1110 resolves the compiled expressions. An object identity loses the distinction between constant and parameter uses.

Fix: identify active parameter bindings in the material resource actually used by this world, feature level, quality, and static permutation. Preserve the complete FMaterialParameterInfo, including association and layer index; use SetTextureParameterValueByInfo. UE/Source/Runtime/Engine/Public/MaterialShared.h:498–500 and :2023–2031 expose relevant compiled binding information, and MaterialInstanceDynamic.cpp:237–248 implements the ByInfo setter. Verify feasibility within the permitted module boundary; otherwise refuse unsupported cases. Reject unavailable/default shader paths before inferring eligibility. Add a fixture containing the inactive-parameter/active-constant alias and a material-layer parameter. A post-set parameter readback alone cannot prove active shader use.

**P1-2 — An identity copy of mip 0 is not an identity texture resource.**

Location: PLAN:144–146, 259–262, 419–428, 457–464, 560–570, 805, 897.

Failure: an authored or alpha-coverage-preserving source mip chain can have the same mip 0 as the RT and different lower mips. A close, front-facing G-ID passes; at distance the masked silhouette or material detail changes with identity still selected. Changing the texture's filter/LOD behavior can likewise change grazing-angle sampling. The plan copies address modes, but does not specify matching the source's effective filter, mip availability and sampling behavior. G-ID at full source size also does not validate the production 1024 cap: a larger source still acquires an extra resolution loss.

There is a second uncovered format case: the compression denylist omits TC_HalfFloat and TC_SingleFloat. They fall into data and are copied into normalized RGBA8. Values outside [0,1] are clipped, and precision is lost even inside that range. A passing BC5/albedo rock fixture says nothing about these formats.

Engine evidence: UE/Source/Runtime/Engine/Private/TextureRenderTarget2D.cpp:648–655 selects filtering using the RT's settings/device profile; :706–713 generates new mips. UE/Source/Developer/TextureCompressor/Private/TextureCompressorModule.cpp:1570–1599 includes sharpening, border and alpha-coverage policy, while :3237–3241 supports existing authored mips. UE/Source/Runtime/Engine/Classes/Engine/TextureDefines.h:343–346 includes the omitted float compression settings.

Fix: specify a supported pixel-format, sampling and mip contract, rather than a compression denylist plus unconditional autogeneration. Preserve the relevant source behavior or refuse cases that cannot be preserved. Maintain both dimensions and aspect ratio; do not silently downsample to fit the budget. Qualify the actual production allocation/configuration at magnification, minification, grazing angles and mip transitions, including alpha coverage, normal detail and shared samplers. Test transformed outputs too: baking tile ×8 into a fixed-size RT has its own spatial bandwidth limit even if identity passes. A narrow passing fixture remains evidence for that fixture, not arbitrary cooked host content. If this cannot be solved through the permitted runtime interfaces, restrict/defer the affected support instead of claiming universal fidelity.

**P1-3 — D1's force-residency addition contradicts target isolation and leaves “full” undefined.**

Location: PLAN:558–574, 654–669, 897; RULING:19–23.

Failure: target A and visible object B share a texture. Forcing it resident for A changes the texture resource used by B too; it may also put pressure on other streamed assets. The original plan correctly identifies this at PLAN:572–574. A force call also changes shared residency state, so the later claims that m53 mutates no shared asset and needs no reverse interaction handling no longer follow. Checking m52 once before a deferred preparation does not reserve the texture against another hold during that preparation.

Cooked APIs exist, but IsFullyStreamedIn is not a proof that cooked mip index 0 is resident: UE/Source/Runtime/Engine/Private/StreamableRenderAsset.cpp:318–329 explicitly permits LOD bias and returns true for some non-streamable/invalid-state cases. Missing optional mips are another case. SetForceMipLevelsToBeResident at :205–218 changes a texture-wide timer/cinematic setting. WaitForStreaming at :356–367 calls the unbounded polling/FlushRenderingCommands/Sleep loop at :334–353; it has no caller-specified timeout.

Fix: keep the full-resolution requirement, but make the initial production policy “already resident at the required cooked top mip, otherwise refuse not_fully_resident.” At synchronous Apply, the safe wait for this policy is 0 frames / 0 ms. Check a valid resource, actual resident mip index and dimensions, and pending streaming/init state; record snapshot_mip relative to the cooked platform chain, with dimensions. An import mip stripped by cooking cannot be restored at runtime.

If yield requires prefetch, return that as a separate design decision: a non-blocking preparation phase outside labelled capture, explicit frame AND monotonic-wall-time cutoffs, identical preparation in clean controls, ownership of changed residency settings, m52 exclusion throughout preparation/copy, and cleanup on timeout/cancel/EndPlay. Do not call WaitForStreaming from Apply or invent a universal “safe” positive timeout without host measurements. Never start a labelled event while preparation is pending. Continue to reject streaming VT resources; their GetNumResidentMips value does not report ordinary residency (Texture2D.cpp:455–479).

**P1-4 — The proposed host-MID clone loses state, and its animation exception violates purity.**

Location: PLAN:125–127, 182–184, 909, 946; RULING:35–37.

Failure: a host MID overrides a runtime virtual texture used elsewhere in its graph, while an ordinary 2D albedo is the m53 target. CopyParameterOverrides clears the clone's overrides and copies vectors, double vectors, scalars, textures and fonts, but not RuntimeVirtualTextureParameterValues. The new MID falls back to its parent's RVT even on the identity path. Independently, a hit flash or animated roughness written to the host MID stops affecting the rendered clone. A host_mid_cloned flag discloses that second change; it does not make it a texture-corruption-only frame.

Engine evidence: UE/Source/Runtime/Engine/Private/Materials/MaterialInstanceDynamic.cpp:484–503 and MaterialInstance.cpp:3502–3517. The engine has an explicit RVT override array and ByInfo setter; RVT not appearing in GetUsedTextures does not make the host's RVT overrides disposable.

Fix: default CloneHostMids to refusal for S1/product use until supported host state is preserved. If clone support proceeds, copy all relevant override types and identities, and define how subsequent host updates are preserved or how such cases are reliably excluded. Simply substituting another bulk-copy API is insufficient without auditing what it copies. Add an RVT-override identity fixture and a host-parameter-animation fixture. Retain host_mid_cloned as useful diagnostic metadata, not as permission to freeze unrelated behavior.

**P1-5 — A failure of Route B does not justify Route A's known reconstruction error.**

Location: PLAN:199–202, 805, 809, 960; RULING:16.

Failure: G-ID detects an unwanted change, then the prescribed fallback deliberately discards host tint, projection, packed-mask routing and shading behavior. G-FID has no rejection threshold. The fallback can therefore “pass” while introducing more non-corruption change than the mechanism that failed. Reconstruction disclosure is not the requested purity.

Fix: remove automatic Route A fallback from D1, S1 and G-ID. If only the normal corruptor fails and all UV paths pass, defer normal_corruption and qualify UV via B. If the failure is in common normal decoding, UV also transforms normals and must not silently reuse or skip that broken path; restrict the applicable materials or defer UV too. Failure in common colour/data/alpha support similarly requires a supported subset or a hold. A takeover can only be a separately approved change in product semantics, not recovery under the same label contract.

**P2-6 — AlphaComposite drift must clear before every redraw.**

Location: PLAN:433–445, 471–479, 565–566, 896.

Failure: the plan derives correct RGB/alpha after a clear, then costs the extra clear only at Apply. DrawMaterialToRenderTarget does not clear on each subsequent draw. With opacity = 1 − source alpha, the blend equations are RGB_new = source RGB + RGB_old × source alpha and A_new = A_old × source alpha. An opaque source with RGB 0.4 produces 0.4, 0.8, then clipped 1.0 over three draws without a fresh clear. This is brightening/saturation in addition to drift. For fractional alpha, the alpha also decays.

Evidence: UE/Source/Runtime/Renderer/Private/BasePassRendering.cpp:276–278; UE/Shaders/Private/BasePassPixelShader.usf:1881–1883; UE/Source/Runtime/Engine/Private/KismetRenderingLibrary.cpp:202–214. The arithmetic was reproduced in memory; no rendered result is claimed.

Fix: explicitly clear the output to (0,0,0,1) before every AlphaComposite draw, including all drift updates, then generate mips. Keep snapshot and output distinct. Add a repeated identity-redraw gate with opaque and fractional-alpha inputs; it must remain stable across many frames. Include these clears and mip generation in drift cost.

**P2-7 — Exact restore needs raw slot state and strong references to the originals.**

Location: PLAN:614–637; PLUGIN/Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.h:27–34 and .cpp:114–120.

Failure 1: the m17 pattern proposed for copying stores OriginalMaterial weakly. Cloning from HostMID->Parent removes the component's reference to HostMID. If the component was its only owner, GC during the event can collect it; reverting then resets a default rather than restoring the original runtime material. The proposed strong-reference arrays name the new MIDs and RTs, not the displaced originals.

Failure 2: on a Nanite component, GetMaterial returns the resolved Nanite override, not necessarily the material stored in the slot. If the explicit slot holds A with Nanite override B, saving GetMaterial saves B. Restoring B loses A. Checking only B for a Nanite override can also miss the intended nanite_override refusal. UE/Source/Runtime/Engine/Private/Components/StaticMeshComponent.cpp:2655–2675 performs this substitution.

Fix: retain the displaced original material objects strongly until restoration/abandonment completes, while keeping actor/component references weak. Capture raw explicit override/default binding separately from effective rendering material, and inspect Nanite routing before cloning. Restore only a slot still owned by this event. Keep host replacement and component-recreation outcomes explicit; a default-reset sweep is cleanup, not proof of exact restoration. Extend G4 with GC during a live clone, explicit Nanite override, foreign material replacement, and both ids' independent resource ownership.

**P2-8 — “Every map coherently” conflicts with partial eligibility and the texture cap.**

Location: PLAN:162–163, 256–262, 277–285, 302, 597–604.

Failure: a material has nine spatial maps, or mixes parameterized albedo with a constant-node normal map or a VT detail map. The plan transforms the eight largest/eligible maps and silently leaves the rest spatially unchanged. On a DXT5-normal host the UV family expressly skips normals. This no longer implements the promised coherent mapping of that slot. A small texture is not necessarily constant; the <64 rule can exclude a patterned mask.

Fix: distinguish intentionally untouched material slots from an incomplete map set within a selected slot. Define atomic eligibility for the advertised coherent-UV mode: prove the required map set can be transformed within the format/residency/memory limits, or refuse that slot/event. Alternatively bring an explicitly partial texture-transform contract back to Chat for a ruling and matching labels/tests. Do not quietly relax coherence to improve yield. Rename the size floor as a policy limit; do not claim size proves constant content. Count exclusions deterministically.

**P2-9 — The refusal vocabulary is neither the stated size nor a deterministic decision tree.**

Location: PLAN:275–303, 982–984, 946; RULING:20–23, 37.

The closed table contains 13 named target reasons, counting no_normal_map and normal_unconnected separately, not 14. Route A text adds others outside that table. More substantively, the candidate set already excludes non-parameters at PLAN:277–278, and the zero-candidate rule fires no_eligible_texture at :282; without preserved intermediate counts/precedence, texture_not_parameter at :297 is swallowed by the generic case. Slot-level nanite_override and texture-level exclusions do not yet have a defined event-level aggregation rule.

Failure: an office run with absent corruptor assets/shader resources, cloning disabled, or no resident source either has no named refusal/counter or is folded into a generic zero. A census cannot distinguish unsupported content from an installation/readiness failure. not_fully_resident and host-MID refusal must also be incorporated.

Fix: publish one ordered decision tree with separate per-texture/per-slot diagnostic counts and exactly one final event reason. Include asset/resource readiness, unsupported encoding, host-MID policy and residency failures, with precedence. Check the active material resource before treating empty GetUsedTextures as texture absence; UE/Source/Runtime/Engine/Private/Materials/MaterialShared.cpp:926–940 exposes the silent empty-expression fallback. For any remaining ambiguous case, report the ambiguity explicitly. Supply a fixture/controlled path for each reachable reason, and mark absent cases UNEXERCISED rather than reporting a passing zero.

**P2-10 — Several proposed gates can be green without qualifying the promised behavior.**

Location: PLAN:781–820, 960–963.

Failure: the positive twin solid_swap is requested at MainWorld's SM_rock, but the unchanged implementation permits only CB_GateLevel or L_ShooterGym plus the fixture flag. It will refuse on MainWorld. The same limitation applies to null_effect references used in G2. G-ID in S1 runs uv_corruption identity; that does not exercise the distinct opaque M_CorruptTex_Normal graph. G-LYRA accepts any named refusal, so both families can be completely inert there and the row still be satisfied. G-COST measures/reports the first-use hitch without a rejection rule, and S3 has no stop condition.

Evidence: PLUGIN/Source/AnomalyInjector/Private/Anomalies/Anomaly_ChangeCase.cpp:12–26; PLAN:789, 807, 816, 820, 960–962.

Fix: declare which rows are qualification gates, diagnostic readings, or owner decisions. Provide actually runnable matched controls per fixture, such as a specifically declared existing material-swap control on MainWorld, without casually widening fixture-only cases. G-ID must run identity through both corruptors and every supported encoding/configuration; absence of alpha coverage is not an alpha pass. Add a deliberately wrong copy/encoding control that must fail G-ID. NoApply cannot falsify identity fidelity because no application is itself an identity result. Require counted successful applications for functionality gates and explicit minimum coverage; all-refused remains a yield result. Predeclare cost comparison method and acceptance/hold criteria, or keep cost acceptance visibly pending for Chat/owner.

**P2-11 — Shader prewarm is not packaged PSO warmup or proof of a completed copy.**

Location: PLAN:391–417, 498–519, 816, 894–897, 949.

Failure: m47 calls EnsureIsComplete. Its body is editor-only, so adding a material to that list does not move a cooked build's first RT PSO creation before capture. Off-screen draws still use the same renderer/RHI scheduling and can stall captured frames. “No FlushRenderingCommands, so the game thread never blocks” and “negligible once” are not established. The current GatherAnomalySwapMaterials helper is shared by prewarm and CountIncompleteAnomalyMaterials; adding two materials to it without separating the callers also violates D6.

Engine/plugin evidence: UE/Source/Runtime/Engine/Private/Materials/MaterialInterface.cpp:1453–1480; UE/Source/Runtime/RHI/Private/PipelineStateCache.cpp:2067–2094; PLUGIN/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:2399–2425 and :2501–2541. DrawMaterialToRenderTarget is void and has early-return paths for invalid world/material/RT/resource (KismetRenderingLibrary.cpp:152–178).

Fix: separate the prewarm-only material collection from the per-frame checker. Define copy readiness and transaction ordering before binding/held labels; a non-null RT or a void draw call is insufficient validation. Explicitly cover both corruptors and target formats in first-use qualification; there can be multiple PSOs. Either qualify actual pre-capture draws/readiness through permitted interfaces or retain and measure the cold-first-fire cost with an acceptance decision. Compare m53 off/on with m55 held constant, and measure capture throughput/frame-time effects, not just m55's own timing statistic. Include worst-admitted map count, drift, clear/mip work, cancellation and repeated event allocation/release. Do not introduce additional module dependencies implicitly.

**P3-12 — strength_class can only be a documented mode prior.**

Location: PLAN:694–728, 975.

Failure: a >=64 constant or permutation-invariant texture passes the size floor and can remain unchanged under tile/scramble. A flat normal can remain unchanged under sign flips; lighting also controls the normal signal. Thus scramble seams are not content-independent, and invert is not per-event “strong” just because the mode is chosen.

Fix: remove those universal statements. A declared expected mode class is acceptable if explicitly distinguished from per-event pixel change and perceptibility; a name such as expected_strength_class would help, subject to the schema ruling. G-STR can amend mode defaults using offline measurements. Keep m55 out of runtime eligibility, ranking, seed decisions, label verdicts and observable semantics.

**Answers and validation disposition**

Q1: the basic one-clear AlphaComposite derivation is sound on the inspected non-Strata base-pass path: RGB is copied and alpha is recovered from destination alpha = 1. The colour sRGB read/write/read scheme is sound for supported sRGB formats, provided format/flags are set before resource creation. The BC5 path reads RG, maps to [-1,1], and reconstructs positive Z; re-encoding XY is the correct channel convention. UE/Shaders/Private/Common.ush:1373–1384 confirms RG versus AG, and UE/Source/Runtime/Core/Private/HAL/ConsoleManager.cpp:2842–2852 makes Compat.UseDXT5NormalMaps read-only. Refusing unimplemented AG support is appropriate. Shared host samplers do override the RT's address/filter choice (MaterialUniformExpressions.cpp:963–975). None of this proves mip-chain identity, precision sufficiency, or drift stability. A G-ID pass at one pose can coexist with a larger unwanted change at another distance/angle (P1-2). The >8 statistic is a tolerance test, not byte identity; small texture errors can cross opacity thresholds or affect grazing normal lighting.

Q2: the residency API is cooked-available; unbounded waiting in Apply and target-wide isolation assumptions are not safe. Use the policy in P1-3. Streaming VT rejection is correct. Runtime VT must also be preserved as unrelated host state when cloning (P1-4).

Q3: discovery should use active bindings, not object-pointer overlap. SRGB is an encoding hint, not proof of BaseColor use; a mask may be sRGB. Route B does not need to identify “the base colour,” which is an advantage. IsNormalMap identifies the compression convention, while IsPropertyConnected only says an input is connected and cannot establish a particular texture's contribution. There remains no promise that an active corruption will be perceptible. Resolve the structural no-op, format, partial-map and refusal issues in P1-1/P1-2/P2-8/P2-9.

Q4: per-event MIDs/RTs, no sharing across actors, ownership-checked restore, EndPlay notification plus weak-pointer polling, and cancellation/FinishRun cleanup are sound foundations. They need the raw-slot, original-reference, clone-state and pending-preparation fixes above. Both auto and FireSpecific paths exclude an actor already in LiveFires (PLUGIN/Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp:286 and :459–473); raw ApplyAnomaly dispatch at AnomalyInjectorSubsystem.cpp:705–728 does not itself enforce that invariant. Specify that supported fire paths preserve G30; do not claim arbitrary direct-console overlaps with missing_texture/corrupted_texture are safe. m52 held/restoring queries must be texture-wide. The reverse auto m52 shared-user refusal is reasonable for untouched textures, but targeted m52 intentionally bypasses it and is not evidence of isolation. Source preparation must not race a new hold. Slot pointer restoration and release of owned references do not prove byte-identical future frames or synchronous GPU deallocation.

Q5: m55 as measurements only, with offline default-mode calibration, agrees with m55 v1. NoApply is a useful real falsifier of the installation/condition predicate if it uses the normal code path and actually checks live slot and parameter ownership. Do not special-case the lever to fabricate condition_held=false; it must follow from the absence of an installed effect. It cannot test active binding correctness, material-copy fidelity or GPU resource success by itself. The gate-specific disposition is below.

| Gate | Disposition before qualification |
|---|---|
| G0 | Useful eligibility census; disagreements are findings, not an automatic pass. Include active binding identities, actual format/size/residency, raw/effective slots and exclusions. |
| G-ID | Can fail at a measured fixture, but needs both graphs, supported formats/configurations, mip/filter/alpha matrix and a bad-copy falsifier. No Route A escape. |
| G1 | Can fail onset alignment on modes/fixtures with a real differing frame. Keep drift's declared gradual onset separate; require successful applications and both tick orders. |
| G2 | Retain NoApply and non-vetoed injected event / empty affected_frames expectations. Use a valid matched null control on each map; ensure the predicate follows actual state. |
| G-STR | Diagnostic/product decision. Report mode and fixture context plus valid null/positive twins; no automatic perceptibility verdict or runtime feedback. |
| G-FID | Disclosure alone cannot qualify purity. Remove from the production fallback path. |
| G3 | Can fail bounded restore comparison. >8 inside onset mask does not prove byte identity or exclude changes elsewhere. Check all touched slots and relevant pre/on/post mask regions against a matched deterministic control; preserve stronger claims only when actually tested. |
| G4 | Can fail lifecycle ownership/cleanup. Add GC, foreign replacement, recreation, pending preparation/copy cancellation and multi-id cases. Distinguish dropped references from completed resource release. |
| G5 | Retain held AND restoring tests, including pending preparation and reverse ordering. State targeted m52 bypass explicitly. |
| G6 | Require real Nanite use, raw override detection, and the revised clone policy. UNEXERCISED override/clone cases do not qualify support. |
| G7 | Yield census/owner input; an all-refused result is possible and must remain visible. |
| G8 | A proper schema comparison can fail. Include new reason counters, snapshot_mip and host_mid_cloned only on appropriate entries; preserve schema-2/P6 policy. |
| G-COST | Needs declared comparison/acceptance method or an explicit unresolved acceptance decision. Include cold PSOs, maximum admitted RT work and drift; m55's own cost is not total m53 cost. |
| G9 | Arithmetic/coverage validation only. Require evidence rows and expected windows/events; exit 0 on absent evidence cannot qualify a feature. NO-TRACE is a strength reading. |
| G10 | Both editor and game builds can fail; retain warning requirement and permitted module boundary. |
| G11 | Non-null loads and one successful draw cannot cover both graphs/encodings. Exercise both assets and packaged shader paths, including noise dependency; missing support must refuse loudly. |
| G-LYRA | Named refusal is a portability/yield reading. Require a successful counted event per id to claim functionality there; otherwise mark that id unqualified on that host. |

Q6: the 5.6 MB estimate is correct in decimal units for one 1024-square full RGBA8 chain: 5,592,404 bytes, or 5.333332 MiB. The plan already counts two RTs for drift. A 2048-square chain is 21.333332 MiB; a 4096-square chain is 85.333332 MiB and cannot fit even alone under 64 MiB. Thus full-resolution D1 and the production 1024 cap must be reconciled by refusal/budget policy, not silent resizing. For rectangular textures sum 4 × max(1,W>>i) × max(1,H>>i) over the actual mip chain. Reserve all required outputs and drift snapshots before committing slots. Count any extra physical copies and pending releases in the appropriate peak metric; a per-event texel cap is not a total VRAM guarantee. UE/Source/Runtime/Engine/Private/TextureRenderTarget2D.cpp:68–78 counts base-level bytes in CalcTextureMemorySizeEnum, so that helper alone would undercount this budget. Cold PSO and per-frame costs remain unmeasured (P2-11).

Q7: hard CDO references, CanContainContent and rebuilding the editor target before cook are the right G45/G47 approach. Ensure the noise texture is referenced by the cooked graph or an explicit hard reference, and that all intended graph branches exist in S1 if later stages are parameter-only. “One cook” can describe one content authoring iteration, not one packaged artifact for every host: Lyra and each office project need their own correct editor/game build and cook against the revised assets. A startup non-null log is diagnostic, not evidence that the required shader permutation rendered. Keep ordinary eligibility refusals separate from missing assets/resources, validate host feature level/RHI/normal convention locally, and keep all-VT/all-nonparameter yield failures visible. No office incidence or performance is established by this review.

Q8:

| Ruling | Review |
|---|---|
| D1 | Agree with provisional B and full-resolution intent. Disagree with force-residency during Apply and automatic A fallback. Accept only after P1-2/P1-3/P1-5 are resolved. UV-only B is a valid reduced scope only if all of its own required paths qualify. |
| D2 | Agree with scramble as the texture-space mode; remove channel1 fallback with A. Specify a true deterministic permutation, not a hash with possible cell collisions, if “permutation” is the promised operation. |
| D3 | Agree with separate ids. Keep state and cleanup isolated for the two registrations. |
| D4 | Agree with default OFF and owner-controlled default-on after yield/strength evidence. All-refused data must not be presented as support. |
| D5 | Agree provisionally with the proposed sets. Keep drift conditional on repeated-draw fidelity/cost, and re-rule using contextual G-STR evidence. |
| D6 | Agree with prewarm-only inclusion and no expansion of the per-frame readiness count. Separate the shared helper; do not equate shader completeness with packaged PSO readiness. |
| D7 | Disagree with default admission of arbitrary host MIDs under the present purity contract. Use refusal until clone/state-following support qualifies. The visibility key remains useful but cannot cure unrelated frozen/lost state. |

**Bounded remainder and next step**

No pixel, alpha, mip/filter, Nanite, lifetime, yield, cost, or office gate was executed. Those are the remaining qualifications after the design is repaired; the source findings above are sufficient to require revision without a game launch.

Code should revise the plan, retaining the two-id/default-OFF direction while resolving the active-binding contract, texture fidelity/residency limits, host-MID policy, exact restore, repeated draws and gate coverage. Chat review REQUIRED to accept that concrete revision before S1. After acceptance, the corrected S1 identity/lifecycle evidence is the next stop before S2; default-on remains the owner's decision after G7 and G-STR. No implementation or release GREEN is issued here.

VERDICT: CHANGES REQUIRED

