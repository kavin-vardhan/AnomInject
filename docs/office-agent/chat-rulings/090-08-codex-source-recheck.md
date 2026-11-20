<!-- GDP Relay run _relay\runs\2026-10-01-090-08-source-recheck | model gpt-6-astra | effort max | duration 1702 s | collected verbatim below -->
090-08 — Independent source re-check of 090-05 and 090-07

**Verdict: CHANGES REQUIRED.** F1's original early-raw-revert case is repaired, but F1 is not closed for partial replacement, component/material-topology changes, or all capture paths. The Nanite filter preserves the intended random-draw sequence when it preserves the candidate set, but it is neither exhaustive nor fail-closed. Kit 1.1 improves ordinary interrupted events and preserves uninterrupted numerical results, but can pass a mistimed re-install without judging it. The Shipping module exclusion and the user-declared vtable helper are acceptable within the qualifications below.

**Chat review: REQUIRED before the proposed merges.** This is evidence and proposed remediation for the orchestrator, not an instruction that any change has already been accepted.

I reviewed the frozen delivery tree at 2015cfa and fix tree at c29e223, the supplied changes from da902d6 and 0347e8c, the previous review/rulings, the 090-05/090-07 reports and relevant journals, the saved 090-07 UBT exports and kit regression work, and the relevant local UE 5.1 engine source. I did not build, cook, launch an editor/game, capture, stage binaries, modify a repository, or write to the bench. I ran only offline Python source/evidence checks and synthetic PNG fixtures inside this run folder. The 090-06 harness remains **NOT RUN**; its preparation and dry run are not results.

Source citations use these unambiguous prefixes:

| Prefix | Location |
| --- | --- |
| F/ | source-feat-2015cfa/ in this run folder |
| X/ | source-fix-c29e223/ in this run folder |
| E/ | D:/UESource/UnrealEngine/Engine/ |
| R/ | D:/IntrusiveAnomalies/_reviews/ |
| M/ | D:/IntrusiveAnomalies/_mailbox/to_chat/ |

Unless explicitly qualified as reproduced offline, the counterexamples below are deductions from the source, not claims of newly captured gameplay. The two placeholder-anomaly implementations, the descriptor, and the office kit are byte-identical between F and X. m53 itself exists only in F. Branch parity and saved-export checks are recorded in source-review-checks.log:1–30.

| Requested question | Answer |
| --- | --- |
| 1. Is F1 closed, with no path left open? | **No.** The narrow raw-revert repair works; findings R1–R4 remain. The interrupted-m53 exclusion uses the correct live state and reserves retained/pending entries in an active capture. |
| 2. Is the Nanite skip leak-free, with the RNG contract intact? | **No / qualified yes.** R5–R6 show leaks and an absent-probe fail-open; R11 shows a non-Nanite render route wrongly removed from the pool. Identical classified inputs preserve draws; AllowNanite=1 does not make all new outputs byte-identical to the old revision. |
| 3. Shipping exclusion and vtable helper? | **Accept the code changes.** The saved graphs support exclusion of all five modules and unchanged Development/Test graphs. This is not a Shipping compile or proof that arbitrary host references/content disappear. The helper body matches the engine macro exactly. |
| 4. Is kit 1.1's interrupted rule sound? | **Not as an unconditional release gate.** Ordinary complete examples work; R7 demonstrates a false pass and R8 a mixed-reason bypass. One-frame gaps can be censored. Uninterrupted numerical behavior is preserved by construction. |
| 5. Do all README corrections now match? | **No.** Several corrections are sound, but stale timing/transition claims, a premature capture-proof claim, and overbroad Nanite/Shipping wording remain (R9). |
| 6. Anything else? | Capture-frame binding, stale override cleanup, the unsupported manual re-apply route, and the per-call/per-frame cost mismatch deserve explicit treatment below. |

**Findings**

**R1 — HIGH — “Every captured slot still holds our material” is not equivalent to “the picture still contains the effect.” Both branches; all four requested texture types on F.**

For missing_texture and corrupted_texture, one live captured slot holding a foreign material immediately makes IsVisualConditionHeld return false, even if all other slots still visibly carry the anomaly. The new shared policy then removes the whole event from annotation membership and turns the entry into effect_interrupted. Mask arming uses the same installed result. A two-slot object with one slot replaced by a damage material therefore becomes a negative while its other half still shows checker/pink. The m53 predicate similarly requires every recorded slot and every RT binding to hold; one lost slot/binding suppresses the entire uv_corruption or normal_corruption event.

Evidence: F/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:267–280 and Anomaly_CorruptedTexture.cpp:267–280 (same paths/lines in X); F/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1414–1446; F/Source/AnomalyInjector/Public/AnomalyLabelSync.h:64–90; F/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:740–760; F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:1467–1482, 1525–1528.

This is directly relevant to the foreign-replace lever: F/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1236–1251 replaces the **first** committed slot and breaks. On a multi-slot event it creates this partial-effect state. The same problem occurs without any bench lever when host code replaces one material.

Proposed fix: represent complete, partial, absent, and unknown installation separately. A partially surviving effect needs a truthful positive/partial contract and appropriate footprint, or all remaining owned changes must be removed before the event is made negative. Merely changing the predicate from “all” to “any” would not solve R3 or establish an exact mask. The present conservative “not fully installed” decision can justify excluding a frame from a strict dataset; it cannot justify calling that frame a clean negative.

**R2 — HIGH — Revert can leave dormant checker/pink overrides that reappear after the label has ended. Both branches.**

A concrete sequence is: apply to a movable mesh with two material slots; replace its mesh asset with a one-slot asset while retaining the component; revert; later switch back to an asset using slot 1. Revert skips the saved slot whose index is now outside GetNumMaterials(), and its owner sweep also visits only GetNumMaterials() slots. The override at index 1 survives. Captured state is then cleared and bActive becomes false. When the asset uses that index again, the checker/pink returns with no active event or label.

Evidence: F/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:184–190, 231–252; identical control flow in Anomaly_CorruptedTexture.cpp at those lines and in X. UE's SetStaticMesh changes the asset and dirties render state without clearing the override array (E/Source/Runtime/Engine/Private/Components/StaticMeshComponent.cpp:2164–2224). GetNumMaterials reads the asset's material count at :2617–2627; GetMaterial returns an existing override at :2655–2667.

This is more than a sampling uncertainty: an owned override can outlive the event and become visible on a later asset change. m53's cleanup sweeps OverrideMaterials.Num(), rather than just the current asset's material count, at F/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1377–1385, so this particular dormant-index hole does not apply to its direct owned MIDs.

Proposed fix: remove or restore every owned override, including indices outside the current asset's material count, while preserving foreign overrides. Add a focused two-slots → one-slot → revert → two-slots regression, checking both the array and the eventual material used.

**R3 — HIGH — The installed predicates inspect the historical slot set, not the component/section set that now renders. Both branches; m53 additionally on F.**

The checks do not establish current registration, visibility, mesh identity, active LOD section usage, or replacement-component ownership. For example, the host can hide/unregister the original component, keep that UObject and its anomaly overrides alive, and render a clean replacement component on the same actor. The predicate remains true, and the clean replacement supplies an on-screen actor box. Conversely, a recreated component can inherit the anomalous material while the old weak component pointer becomes invalid: the placeholder predicates can return false when no historical component remains, and m53 records the missing slot as false, although the successor still renders the effect.

Evidence: F/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:255–280 and Anomaly_CorruptedTexture.cpp:255–280; F/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1423–1438. The placeholder predicates skip invalid weak components, but do not use the successor lookup employed by revert at :167–174. The m53 test reads raw OverrideMaterials pointers rather than the current effective material/sections. Actor projection is independently obtained through F/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:698–712.

Ordinary LOD changes that continue using the originally covered slots are not intrinsically broken. The uncovered cases are changed slot use, mesh/component replacement, and streaming that changes that render footprint. UE's material lookup can still return an unused override after an asset change (E/Source/Runtime/Engine/Private/Components/StaticMeshComponent.cpp:2617–2677), so pointer equality alone cannot establish a rendered effect.

Proposed fix: track component/mesh generation and the current affected render footprint, and revalidate that footprint at the frame boundary. Treat unresolved topology changes explicitly; do not infer “effect present” or “effect absent” from historical pointers alone. Include component recreation and visibility/LOD/material-topology cases in the runtime validation.

**R4 — HIGH — Label inputs are not consistently bound to the captured picture. Both branches; inherited paths still invalidate an unconditional F1 closure.**

There are two concrete paths:

1. **Asynchronous completion uses live actor geometry.** Installed state is frozen at world-tick end, but FillAnnotationInputs runs when an older readback completes and recomputes FireOnScreen using the actor as it exists then. BuildFrameLabelRecord also projects that live actor again. If frame n contains the effect, and the actor is destroyed or moves out of view before readback completion, frame n can lose its annotation despite still showing the effect. Motion in the reverse direction can produce a box/membership for a place where that frame did not show the actor.
2. **The synchronous path reads current game state with the already-rendered viewport.** CaptureCurrentFrame is called from the subsystem tick. It samples the new installed state, then calls ReadPixels without requesting a matching new scene draw. UE's game loop ticks the world before RedrawViewports. Thus an apply/revert during this tick can be paired with the preceding viewport picture. This path is exposed by IAI.Capture.Async 0 and is also the explicit asynchronous-rectangle fallback.

Evidence for (1): F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:5880–5927, :4812, :7946–7958; X equivalent :5795–5842, :4739, :7681–7693; F and X/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:182 and :698–727. Saving the view and an actor position does not freeze the actor's bounds used by these functions.

Evidence for (2): F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:1009–1094, :1917–1923, :5312–5314, :5343–5367; F/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:630–665; F/Source/AnomalyCapture/Private/AnomalyPreviewCapture.cpp:12–43. Engine order: E/Source/Runtime/Engine/Private/LevelTick.cpp:1606 and E/Source/Runtime/Engine/Private/GameEngine.cpp:1775, :1891.

The normal asynchronous world-tick-end sample is an improvement and ordinarily precedes rendering. It is still a CPU installation sample, not an immutable render-side observation. Later host callbacks can change render state; UE exposes a later OnWorldPreSendAllEndOfFrameUpdates callback at E/Source/Runtime/Engine/Private/LevelTick.cpp:1054.

Proposed fix: freeze membership geometry and installation/event identity with the same capture request/view family that supplies the image and mask, and consume those frozen values at completion. The synchronous route needs matching render completion/state history, or an explicit unsupported/unjudgeable status for timing-sensitive delivery; it should not silently claim the same timing guarantee.

**R5 — HIGH — “Any Nanite part” is not what the shared filter actually enumerates, and qualification is not maintained during an event. Both branches.**

ActorDrawsAnyNanite first calls IsRenderableComponent, which applies **target-selection exclusions**. A visible Nanite component excluded by a component-name or mesh-name pattern is skipped even if the actor's other, non-Nanite mesh keeps the actor eligible. The actor can then be selected, fired, and labelled. All admission sites, including E7N, share this filtered helper, so the backstops do not close this route. In an unscoped material application, the injection can still touch that excluded Nanite mesh.

There is also no continuing Nanite check in the frame-membership path. A component added after apply, a hidden Nanite component made visible, or a mesh/bDisallowNanite change can turn an admitted actor into a mixed Nanite target while its existing event remains labelled. The mask cannot represent the Nanite part.

Evidence: F/Source/AnomalyInjector/Private/AnomalyViewport.cpp:98–150, :791–825, :1416–1439; X corresponding IsRenderableComponent at :778 and enumeration at :1370. Admission sites are F/Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp:371 and :634, F/Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp:813–838, and F/Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp:830–855. The frame gate at F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:6022–6053 has no Nanite revalidation. The same absence applies to the shared active-state routes used by stuck_low_mip and the LOD types; those types are not exempt from the admission backstop, but it is only an admission backstop.

The existing unmeasurability check does not rescue a mixed actor: F/Source/AnomalyCapture/Private/AnomalyMeasurability.cpp:57–80 marks the target Nanite-unmeasurable only when **all** counted renderable components are Nanite.

Proposed fix: give “any drawn Nanite part of this target” its own enumeration, independent of candidate-exclusion heuristics. Recheck relevant component/render-route changes while the event can emit labels, including held/restoring m52 events. If eligibility is lost, terminate/restore or explicitly invalidate the affected sample without falsely declaring the still-visible effect clean. Test mixed actors, excluded component names, newly added components, and Nanite toggles through auto, targeted, direct apply, E7N, m52, and LOD routes.

**R6 — MEDIUM — With no registered Nanite probe, AllowNanite=0 fails open. Both branches.**

GNaniteComponentProbe starts null. ComponentDrawsNanite returns false when the callback is null, so zero detected Nanite components is indistinguishable from “classification unavailable.” HasNaniteComponentProbe exists but is not used by admission. The capture module installs the callback only in StartupModule under ANOMALY_CAPTURE and clears it on shutdown. Therefore an injector used before/without that module can admit an actually Nanite target despite setting 0.

Evidence: F/Source/AnomalyInjector/Private/AnomalyViewport.cpp:77, :1401–1413, :1430–1454; X :75, :1355–1367; F and X/Source/AnomalyCapture/Private/AnomalyCaptureModule.cpp:11–24. F/AnomalyInjector.uplugin:32–42 gives injector and capture the Default phase; that arrangement is not an availability check at the call site.

I am not claiming the ordinary fully loaded capture startup necessarily hits this window. The answer to the explicit missing-module/load-order question is nevertheless **fail open**, not fail closed.

Proposed fix: make probe availability an admission prerequisite for setting 0, return an explicit unknown/refusal reason until classification is available, or move the authoritative probe behind an explicit dependency available to every caller. Setting 1 can remain the intentional bypass.

**R7 — HIGH — Kit 1.1 can silently skip a re-install and PASS a mistimed event. Both branches; reproduced offline.**

Use labels 40–47 and 56–63, effect_interrupted on 48–55, and effect pixels only on 40–47 and 57–63. The second label starts one frame too early. Omit the image for frame 54 while retaining its metadata. That image is in the second run's onset reference. The kit returns status OK, release verdict PASS, no failures, and only one edge record (40–47 at 0/0). The mistimed 56–63 run is never judged.

Evidence: F/tools/label_sync_check.py:411–429 returns None for missing or undecodable images; :1131–1152 chooses the interruption reference and executes continue if one reference image is unavailable; :1246–1256 retains only processed runs and accepts event-level measurability from any run; :1278–1317 computes edges and PASS without requiring all labelled runs to have been judged. transition_gate intersects labelled frames with the available series at :861–866, so the skipped run's labels do not force a failure. This kit file is byte-identical in X.

Reproduction: kit_review_checks.py:61–66; kit-review-results.json:33011–33032; the compact result is also in source-review-checks.log. The complete eight-frame-gap control produces two 0/0 edge records and PASS. Removing only a reference image is enough to erase required judging of the second run. This exposes an inherited permissive skip through the new interrupted-run references; it is not evidence that every complete interruption is misjudged.

Proposed fix: require explicit coverage/status for every labelled run and every needed edge. A missing/undecodable reference must yield NO-REFERENCE, ERROR, or CENSORED for that run and prevent an all-runs PASS. Require the number and identity of judged runs to match runs_of(L), and test a deliberately wrong second run with one missing reference image.

**R8 — MEDIUM — An interruption can still receive an AA exception if another reason is present. Both branches; reproduced offline, no ordinary producer path established.**

The reason is correctly absent from AA_ONLY_REASONS. However, transition_gate checks whether the reason set contains any AA reason and does not give effect_interrupted precedence. A fixture labelled 40–47 with 70% residual effect at 48, interruption flags on 48–59, and both effect_interrupted and temporal_aa at 48 yields PASS: raw end +1 becomes adjusted end 0.

Evidence: F/tools/label_sync_check.py:896–918, :1278–1317; kit_review_checks.py:58–60; kit-review-results.json:32213 onward. The README says a frame can have several reasons and the strictest should govern (F/docs/client-readme.md:948).

The present normal FireWindow writer does not ordinarily add temporal_aa to these four texture types: render-truth processing skips other types at F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:7224–7226, and hide transitions require ActorHidden at :7504–7506. This is an evaluator-input robustness finding, not a claim that current normal captures emit that combination.

Proposed fix: forbid AA excuses on an effect_interrupted frame regardless of additional reasons, or reject conflicting reason combinations explicitly. Add this mixed-reason case to the kit's adversarial checks.

**R9 — MEDIUM — The README still contains stale rules and evidence overclaims. Both branches, with additional stale m53 text on F.**

These are material contradictions for consumers, rather than a request for more prose:

| Text/evidence | Problem | Proposed correction |
| --- | --- | --- |
| F/docs/client-readme.md:1045; X equivalent :1040 | The consolidated texture row still says on-screen burst frames, no interruption reason. | Include the installed/partial/unknown behavior ultimately chosen for F1 and effect_interrupted. |
| F/docs/client-readme.md:906–907, :1303–1306 | m53 is omitted from the interruption table; its dedicated section says it never carries a transition flag and every slot of the object's meshes is corrupted. | Include both m53 types; reconcile this section with selected-mesh scope and the new interruption rule. |
| F/docs/client-readme.md:946; X :941 | effect_interrupted is described as “picture shows no anomaly” and safe as a negative. | Until R1–R3 are fixed, say only that the complete recorded installation could not be confirmed; do not promise a clean picture. |
| F/docs/client-readme.md:1167–1168; X :1160–1161 | Claims proof “by a capture with an early revert.” | Remove the capture claim or cite actual completed matching evidence. R/090-06-README.md:1 explicitly says prepared, NOT run. The 090-07 work used doctored copies and synthetic checks, not a newly captured early-revert event. |
| F/docs/client-readme.md:1152–1165; X :1145 onward | “Every labelled event has a real mask,” “at least one Nanite part,” “logged once,” and blanket unmeasured behavior under setting 1 are too broad. | State the verified admission policy and remaining limits. Distinguish all-Nanite from mixed targets. Refusal counting is distinct-object counting; targeted/apply logs can repeat (F/Source/AnomalyInjector/Private/AnomalyViewport.cpp:1458–1460). Masks also require their capture settings/readback support; F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:3613 already makes them conditional. |
| Both docs/client-readme.md:53 | Module exclusion is expanded to “contains none of the plugin.” | Say its five code modules are excluded. Content capability and the enabled WebSocketNetworking dependency remain in both AnomalyInjector.uplugin:13, :51–55. |
| F/docs/client-readme.md:1087–1088 | Says flagged frames are simply left out of the comparison. | Distinguish the specific AA exceptions from partial/unresolved handling and interruption frames, which the new kit deliberately reads and may fail. |

The positive corrections are listed under question 5 below. They should be retained.

**R10 — LOW — F1's timing log measures calls, not the ruled per-captured-frame cost. Both branches.**

The log's mean divides by InstalledChecks, and its maximum is the slowest individual call. Several mask/label consumers can call the predicate for one frame, and the separate ConditionHeld reads are untimed. A low reported call mean/max cannot establish mean <=0.05 ms per captured frame and maximum <=0.5 ms per frame.

Evidence: F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:6044–6051, :5924–5927, :8955–8965; X timed call at :5960 and summary at :8625 onward. The declared threshold is R/090-05-chat-ruling-f1-nanite.md:63–65.

Proposed fix: account for all installed-check work per captured frame and report the distribution of those frame totals, preferably sharing one immutable per-frame result among consumers. No runtime cost result is claimed here, and the ruling explicitly says an over-budget cost is not by itself a delivery blocker.

**R11 — MEDIUM — The probe can reject a non-Nanite spline mesh, narrowing the literal “no Nanite candidate means unchanged behavior” claim. Both branches.**

ComponentRendersAsNanite casts to UStaticMeshComponent and tests the mesh asset's Nanite data, without excluding USplineMeshComponent. In this UE 5.1 source, a spline component creates FSplineMeshSceneProxy, not a Nanite proxy, even when its source mesh asset has Nanite data. bDisallowNanite defaults false and the spline constructor does not change it. With Nanite enabled on the platform, this ordinary raster-rendered target can therefore be classified Nanite and removed by the new setting-0 filter.

Evidence: F and X/Source/AnomalyCapture/Private/AnomalyMeasurability.cpp:25–43; E/Source/Runtime/Engine/Private/Components/StaticMeshComponent.cpp:207; E/Source/Runtime/Engine/Private/Components/SplineMeshComponent.cpp:190–214, :640–651. m53's own usage analysis already distinguishes this case: F/Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp:220–222 and TexCorruptPure.h:618.

The PRNG arithmetic is unchanged, but a physically all-raster candidate pool need not be preserved. If that wrongly refused spline was the only candidate, even subsequent draw consumption follows the no-candidate path. The abstract KAT supplies classifications and cannot detect this misclassification.

Proposed fix: classify the actual component render route, including the UE 5.1 spline exception and the component's scene platform, rather than treating every static-mesh-derived component with a Nanite asset as Nanite. Add a classification-to-selection regression for a spline using a Nanite-enabled asset.

**Answers and accepted portions**

**1. F1 and the interrupted-m53 exclusion.** The original “apply at 10, raw revert at 20, retained auto entry through 39” case is repaired at the state/policy level: IsAnomalyVisualConditionHeld first requires an active instance, and FireWindow membership now requires installed as well as on-screen. Revert restores before dropping normal active state. Annotation membership, labelled, and mask gating now consult that installation condition. Evidence: F/Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp:892–913, :1119–1126; X condition accessor :829; F/Source/AnomalyInjector/Public/AnomalyLabelSync.h:65–90; F/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:715–760. This closes the original stale-live-entry cause under a matching frame sample, not all paths in R1–R4.

For missing_texture/corrupted_texture, the test is approximately “every still-valid originally captured slot has a checker/pink-derived material, and at least one such slot exists.” It is not “every current target slot,” nor a rendered-effect test. For uv_corruption/normal_corruption, the test is “our exact raw MID is still in every owned slot, and our recorded texture parameters still point to the RTs.” It does not read the rendered texture, section, or visible component. Thus each test is useful ownership evidence but not a faithful universal installed-on-the-picture predicate.

Ordinary source-texture streaming is not, by itself, an identified new m53 leak: qualification rejects pending/not-fully-resident textures (F/Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp:432–474), and the effect uses copied RT outputs, enqueued before the slot commit (F/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1023–1044, :1120–1128). Revert restores/sweeps slots before releasing outputs and original holds (:1318–1398). That ordering is sensible. It does not repair component/mesh streaming replacement, prove a matching captured frame, or prove zero AA history.

The foreign-replace/re-install gap now becomes unlabelled while the condition is false, and resumes when it reads true. That is correct for a complete, genuinely clean gap under the timing assumptions. It is wrong for a partial surviving effect (R1). The source does not contain an automatic production “replace and reapply two ticks later” repair: the bench lever makes a one-slot foreign replacement after two ticks (:1236–1251); CommitSlots only commits uncommitted slots (:1144–1159). A re-install is an external/material-state action or another Apply; the kit's re-install examples are doctored/synthetic evidence.

**The specific m53 co-entry hole is closed during normal capture.** Exclusion now reads IsActive(), not visual-condition-held, at F/Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp:935–938 and :1014–1037. A foreign-replaced-but-active m53 therefore still blocks m52. After a raw revert makes it inactive, capture's trail provider continues to reserve retained m53 live entries and pending output: F/Source/AnomalyInjector/Public/AnomalyExclusion.h:156–163; F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:6380–6396, :6446–6472, :6477–6493. Provider registration is at :3650, and run setup clears old live fires at :3432. I found no additional normal-capture co-entry bypass caused by the new interrupted reading.

That conclusion is intentionally narrower than “co-entry impossible in all circumstances.” The existing unresolved-m52 escape deliberately admits m53 while blur may remain and counts it (F/Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp:851–859). Outside capture there is no capture-emitter reservation to claim, and unsupported raw/manual state manipulation is discussed below.

**2. Nanite routes and RNG.** With a registered classifier, stable component set, no selection-exclusion blind spot, and setting 0, the source does reject a detected Nanite target through the auto pool, targeted fire, direct ApplyAnomaly backstop, and m53 E7N. The backstop is for object-scoped anomalies, so stuck_low_mip and the object LOD types share it; camera_clipping's global scope is intentionally unaffected. There is no second independent detector that would close R5–R6.

The setting defaults to 0 and resolves console over ini over compiled default in F/Source/AnomalyInjector/Private/AnomalyDefaults.cpp:1053–1124. The probe tracks the main UE 5.1 Nanite inputs (bDisallowNanite, editor fallback flag, valid Nanite data, UseNanite) at F/Source/AnomalyCapture/Private/AnomalyMeasurability.cpp:25–43. These are CPU classification inputs; the callback is not a query of the exact scene proxy captured, and misses the spline exception in R11.

**The random-draw contract is preserved for the same eligible IDs, candidates and mode inputs.** The new filter consumes no random values and preserves candidate order. On F it is inside CandidateCount, before target/hold/mode draws; the unchanged production DrawAttempt still owns draw order (F/Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp:345–461; F/Source/AnomalyInjector/Private/Anomalies/TexCorruptPure.h:1388–1445). The targeted filter is before TargetedAttempt at :634–655. On X the filter is before the existing target/hold draws (X/Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp:294, :369 onward, :493–503). When the classifier reports no Nanite candidates, the same candidates reach the same draws in the same order; setting 1 preserves the unfiltered set. The stronger condition “nothing actually renders with Nanite” is insufficient as written, because R11 can still change the pool.

The new m53 KAT invokes production DrawAttempt, not only a substitute policy model, at F/tools/texcorrupt_draw_test.cpp:258–305. It checks the old unfiltered answers, filtered index 3 of 5, identical three-draw seed states while candidates remain, and the all-refused path's one ID draw. On m53, “before the draw” must mean before the **target draw**: the ID draw deliberately precedes CandidateCount. I did not compile/rerun the C++ KAT in this review.

**“Setting 1 is byte-identical to pre-change behavior” is too broad.** It restores candidate admission and that draw sequence, not whole-session bytes or every other change in the diff. F1 changes memberships, the label-rule version and interruption reason; run summaries have new Nanite fields/counters; RefuseNaniteTarget still enumerates even when allowing (:1448–1454). Wall-clock timing and serialized evidence are not a KAT guarantee. This does not undermine the narrower RNG conclusion.

**3. Shipping and the vtable helper.** The descriptor is identical in both archives. Each of the five modules denies precisely Shipping (both AnomalyInjector.uplugin:23, :29, :35, :41, :47). UBT compares the actual configuration enum and returns false for the deny list (E/Source/Programs/UnrealBuildTool/System/ModuleDescriptor.cs:478–518), and plugin module creation honors that result (E/Source/Programs/UnrealBuildTool/Configuration/UEBuildTarget.cs:3664–3685). The deny list itself does not exclude Development or Test.

I independently compared the supplied full before/after exports: Development Game, Test Game and Development Editor are byte-identical for both branch builds; Shipping changes from 306 to 302 modules, with no Anomaly modules among the 302 and no remaining-module dependency/include-module reference to them in those exports. Evidence: R/090-07-work/ubt/summary_fix_after.json:3–46 and summary_m53_after.json:3–46; local source-review-checks.log:11–30. These are the two supplied branch-build graphs, **not** new builds of Concorde or Bates.

There is consequently no identified dangling code-module reference in the supplied host graphs. An arbitrary host that explicitly depends on plugin classes/modules or serialized assets still needs its own references made conditional; a module deny list cannot rewrite host code or assets. Content/dependencies remain relevant: UBT recursively adds plugin dependencies and can stage a content-bearing plugin descriptor even with no included modules (E/Source/Programs/UnrealBuildTool/Configuration/UEBuildTarget.cs:3687–3710). Hence the narrow exclusion is safe on the reviewed graphs; “the entire plugin is absent from every possible Shipping host” is unproved. No Shipping compile has been run, and the pre-existing disabled-branch compile defects were excluded rather than repaired.

The helper is equivalent to UHT's normal generated helper body. The exported class declares FVTableHelper& at F/Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h:63–70, and defines it as Super(Helper) with an empty body at F/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:593 (X :594). E/Source/Runtime/CoreUObject/Public/UObject/ObjectMacros.h:1768–1773 declares that same helper and defines exactly that body. Default member initialization remains the same; it deliberately does not run the ordinary constructor's MakeUnique at F :588–591. Locating the definition where the private state is complete addresses the strict-include issue without changing the helper's behavior. No extra fix is proposed for this constructor.

**4. Kit 1.1, G430, limits, and regression.** The main new rule is sensible: interruption frames are known, are not themselves AA excuses, and provide their own local reference. With at least eight consecutive interruption frames, post_int[-6:] avoids using the first one or two possibly mistimed frames as the off reference (F/tools/label_sync_check.py:1143–1149). A following run takes its onset reference from the interruption, and each processed run has its own edges (:1131–1141, :1280–1292). This matches the reasoning in F/docs/gotchas.md:9845–9860 (G430), but “last six” is a reference-selection heuristic, not an independent guarantee that those six are clean. The noise/half-depth comparison at F/tools/label_sync_check.py:1111–1119 and :1307–1309 is useful protection when the gap still resembles the effect.

I exercised 66 offline fixtures using the frozen kit and its stdlib PNG decoder: gaps of 1, 2, 3, 4, 6, 7, 8, 9 and 12 frames; clean and host-material gaps; still-visible gaps; shifted first ends and re-installs; residual AA; mixed reasons; missing reference images. The script and full results are retained as kit_review_checks.py and kit-review-results.json. This is newly run evaluator evidence, not renderer evidence.

For the complete examples with gaps of at least two frames, the tested correct labels pass and the deliberately shifted/still-visible cases fail. The eight-frame correct re-install has two 0/0 edges. The all-visible eight-frame gap fails with “interrupted frames show the effect.” These are useful positive and negative checks, but R7 means “judged per run” is not enforced for all inputs.

The stated one-frame limitation is real and slightly less decisive than the journal wording suggests. A one-frame host-material gap, a one-frame gap that still shows the effect, and a one-frame 70% AA smear each produced **CENSORED**, raw end +1, no ordinary failure, in these fixtures (kit-review-results.json:471–509, :2815–2853; source-review-checks.log also records the still-visible case). The unresolved-edge handling at F/tools/label_sync_check.py:1288–1300 explains this. “fail 0” is therefore not proof of correct timing. The office read-back must retain and interpret its censored count.

An ordinary interruption flag does not excuse a >50% AA smear; a sufficiently observed +1/+2 tail should fail the half-strength rule. A too-short gap can be censored instead, and R8 shows why conflicting AA reasons need a guard. None of this proves absence of sub-half-strength residual pixels. The proposed release rule is a half-strength rule, not a zero-residual-pixel rule.

**Uninterrupted regression holds for numerical judging by construction.** When ev["I"] is empty, pre_int/post_int are empty; the old ron and roff branches are selected; no interruption median check contributes a failure. The shared transition gate is unchanged by this diff. The added counters are zero. This is visible in F/tools/label_sync_check.py:1108–1163 and :1274–1317 and the supplied kit diff.

The saved independent regression output reports eight sessions, 91 event rows, 60 judged events, zero canonical per-event differences (R/090-07-work/regress_old_new.log:1–4). “Identical reports” means **after normalization**: its driver removes version/method/decoded footer text, the new interruption/unknown-reason lines and read-back suffix, then removes new event fields (R/090-07-work/regress_old_new.py:30–39, :55–57). Literal report bytes are intentionally different. I checked that evidence and construction; I did not rerun the eight full banked sessions.

**5. README corrections that do hold.** F2–F5 and LOW were improved substantially, but R9 prevents an unqualified pass:

| Earlier item | Current assessment and evidence |
| --- | --- |
| F2: half-strength rule, AA onset blindness, residuals | The README now explicitly distinguishes half-strength timing from every visible pixel and admits a one-frame flagged-onset blind spot: F/docs/client-readme.md:1084–1104. This is a useful qualification. It does not prove F1's new interrupted paths, and the “all flags left out” sentence still needs repair. |
| F3: event counts and repetitions | Correctly says 16 complete events plus one right-censored event per m53 capture, natural/synthetic AA-on, natural AA-off, and a second natural AA-on run: F/docs/client-readme.md:1123–1128, :1316–1325. The separate mode-oracle fixtures are distinguished from timing fixtures. These agree with the 090-02 reviewed evidence; no new capture count is asserted here. |
| F4: coverage and m52 tail | The exact run matrix, 1280x720/30 fps, TSR versus method 0, plain TAA/other resolutions/office hosts still unrun, and “fell below half strength 4 to 6 frames” are now stated: F/docs/client-readme.md:1109–1138. These appropriately narrow the claim. |
| F5: selected meshes versus whole actor/pixels | The qualification at F/docs/client-readme.md:1259–1270 is good: selected mesh set, late/out-of-view components excluded, and no promise every pixel changes. The contradictory whole-object/never-transition paragraph at :1303–1306 remains. |
| LOW: negative override values | The “any negative = default” wording is consistent with the resolver's negative/default handling; see F/docs/client-readme.md:957–961 and ResolveTransitionFrames at F/Source/AnomalyInjector/Public/AnomalyLabelSync.h:113–123. |
| Nanite trade-off | Fewer eligible targets and possible sparse/empty captures are correctly described at F/docs/client-readme.md:1158–1161. The exhaustive detection/mask claims are not established, for R5–R6 and R9. |
| effect_interrupted | Reason/schema and intended installed gate are present, but documentation has not reconciled all tables, partial effects, or the absence of a real new runtime capture. |
| Shipping | “Capture with Development or Test” and the five-module Shipping exclusion are right. The stronger entire-plugin wording goes beyond source/export evidence. |

**6. Other routes and remaining evidence limits.**

**NOTE — Raw manual re-apply can rebind a retained event to the wrong picture.** The new installed query is keyed only by anomaly ID, not by target/application generation (F/Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp:1119–1126). ApplyAnomaly can replace the instance/active record for that ID (:796–889), while Auto's old live fire still points at the previous actor until its scheduled end. A raw revert of A followed by raw Apply of the same ID to B can therefore make A's retained entry read “installed” because B now holds the effect. The source already calls manual pool-ID injection during Auto unsupported and warns that it clobbers the one-instance-per-ID at F/Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp:959–965. I do not promote this documented unsupported path into a newly introduced normal-use blocker. Proposed hardening is to reject such re-applies while a captured event is retained, or key the installed reading to the event's target/application generation. A universal “no path left open” statement must explicitly exclude or enforce this restriction.

Masks remain target silhouettes, not a proof that every included pixel changed. Weak UV/normal effects, nearly uniform textures, flat lighting, occlusion, temporal history, missing mask measurement and intentionally unconfirmed/global labels remain distinct from installation. The new predicates cannot convert those existing limitations into pixel-truth guarantees. F/docs/client-readme.md:1095–1104 and :1265–1270 already give useful portions of that distinction.

The 090-04 include work and the vtable helper do not present an additional label-semantic regression in the inspected changes. The 090-07 Shipping graphs and unchanged uninterrupted kit results are useful evidence, but neither substitutes for the new F1/Nanite runtime smoke. R/090-06-README.md:1 and the user's explicit status establish that the smoke is still unrun. The captured-frame correlation, partial replacement, topology cleanup and late-component cases need evidence beyond the existing pure predicates and all-slots-retake lever.

I would not rule F1 or the Nanite mask guarantee closed on these heads. The proposed priority is the concrete label/picture defects R1–R5 and false-pass R7, then fail-closed and render-route classification, conflicting reasons, and the documentation corrections. R10 is a measurement correction, not a new performance failure. The Shipping descriptor and vtable helper do not need to be rejected to address those findings.

VERDICT: CHANGES REQUIRED
