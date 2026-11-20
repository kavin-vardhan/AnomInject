<!-- relay: run folder runs\2026-09-28-084-02b-m52-minidelta | model gpt-6-astra | effort max | duration 1105 s (16:54:06-17:12:31) | collected by Claude Code 084-02b, content unedited -->
**084-02b — Independent mini-delta review of d798900 — 2026-09-28**

**Not all items are resolved. The candidate is not fit for tonight's bench under decision 1's “correct first, then the bench” condition.** F3, F7, the requested Shipping source exclusion, and resource-identity classification are repaired. The Apply-time purity scope is repaired. F1, F2, F4's lifetime monitoring, F5 and F6 retain integration defects. The two-sided comparator catches the four specified shifts, but its measurement and mask coverage can still report false passes.

This report is information for chat Claude's ruling. Proposed corrections below are proposals, not accepted implementation orders. The explicitly permitted residual with `IAI.StuckMip.SettleTailFrames=0` is **not** my reason for withholding bench readiness.

The main new integration findings are the opening-mask omission for inherited events, incomplete reopen bookkeeping, and a hard-bound m55 result that can disagree with a later label. The monitoring and gate counterexamples below also go beyond the supplied selftests.

I reviewed Git objects `749a3bd..d7989003d7da6a8eefb6e361e2d225de0a0f8817`: 14 files, +2,217/−201, together with relevant unchanged capture code and local UE 5.1 source. I used the 084-02 review, 084-02 chat decisions 1–3 and the 085-00 ADDENDUM. All six harness hashes match the supplied prefixes. The archived executable hashes to `FD621B27E92DA23A6325532FDF0581C65374EAF7E9F14DDE3ACBD2DBB9C9A3CB`. Its preserved Game/Editor logs show completed linking/metadata and no compiler-warning lines; I did not rebuild or independently reproduce their exit codes. Shipping remains unbuilt. No new runtime evidence was produced.

All writes were confined to this relay folder. No repository/config/index writes, checkout, build, cook, editor/game launch, capture, staging, binary swap or process kill occurred. I executed only an engine-free Python check of selected harness functions, without importing the runner or common module. Inputs and hashes are preserved in [input-manifest.json](D:/IntrusiveAnomalies/_relay/runs/2026-09-28-084-02b-m52-minidelta/input-manifest.json).

The following aliases identify **d798900** files; citations use one-based source lines. `UE/` means `D:/UESource/UnrealEngine/Engine/Source/Runtime/`. Harness citations refer to the verified current bytes copied into `inputs/`. `SI` means captured `session_index`.

| Alias | Repository-relative file |
|---|---|
| Capture | `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` |
| Window | `Source/AnomalyInjector/Public/AnomalyStuckMipWindow.h` |
| Mip | `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` |
| SveCap | `Source/AnomalyCapture/Private/AnomalySveCapturer.cpp` |
| SVE | `Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp` |
| MaskMeasure | `Source/AnomalyCapture/Private/AnomalyMaskMeasure.cpp` |
| Change | `Source/AnomalyCapture/Private/AnomalyChangeMeasurement.cpp` |
| ChangeStage | `Source/AnomalyCapture/Private/AnomalyChangeStage.cpp` |
| Writer | `Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp` |
| Stats | `Source/AnomalyInjector/Public/AnomalyStuckMipStats.h` |
| Tests | `tools/m52_window_selftest.cpp` |

**Resolution ledger**

| Item | Disposition | Evidence and remaining limit |
|---|---|---|
| F1 | **PARTIAL / NOT RESOLVED** | Arm-time pending/restoring checks now exist (`Capture:5680–5697`); unsettled baselines break confirmations (`Window:345–363`). They do not fence the streamer's not-yet-issued requests; see F1 below. |
| F2 | **PARTIAL / NOT RESOLVED** | Ordered receipt cursor, explicit missing heads and closure fence are implemented (`Window:195–298,318–363`; `Capture:6070–6111,6226–6237`). Reopening does not restore masks/m55 and leaves lifecycle bookkeeping stale (`Capture:6028–6059,6241–6256`). |
| F3 | **RESOLVED in source** | Zero textures → Unknown → member (`Window:81–114`). Event texture cache/restoring fallback (`Capture:5530–5583`), final watch extension (`:5703–5758`) and missing-sample classification (`:6372–6410`) remove the unsupported before-Apply inference. |
| F4 / purity | **Apply scope RESOLVED; lifetime NOT RESOLVED** | Loaded and streaming-loaded levels, owned unregistered primitives and decals are included (`Mip:48–177`; `Window:430–439`). New-user monitoring misses ordinary material/registration routes (`Mip:1264–1296,1438–1486`). |
| F5 | **PARTIAL / NOT RESOLVED** | Live cuts are counted/opened (`Capture:6927–6952`); trails carry and rebase (`:5863–5981,7636–7641`). Inherited events lack opening mask records, and an intervening non-render-truth run clears carried ownership on finish. |
| F6 | **PARTIAL / NOT RESOLVED** | Provisional negatives are removed and shutdown closure is deferred (`Capture:6593–6624,6921–6925,6969–6975`). The 64-tick fallback is not made final for the label/mask authority; later live receipts can disagree. Reopening also loses m55 continuity. |
| F7 | **RESOLVED in source** | Deferred age survives move/remove/reinsert (`Capture:1489–1511`); expiration releases the payload and pending mappings and reports unmeasured (`:6293–6318`; `Window:461–464`). |
| Shipping | **Requested source exclusion RESOLVED** | Storage/accessors/legacy branch have Shipping guards (`Mip:35–37,354–364,460–464,471–479,634–648`; `Stats:47–51`). Capture registration remains excluded by `ANOMALY_CAPTURE=0` in `Source/AnomalyCapture/AnomalyCapture.Build.cs:340–359`. This is not Shipping binary qualification. |
| Resource replacement | **Requested classification RESOLVED** | Apply binds `GetResource()` identity (`Mip:767`); capture compares the RT sample, logs and counts mismatches (`Capture:6378–6401`; `Window:63–78`). The affected texture becomes unknown; another known-held texture may still make the aggregate held, conservatively labelled. |
| Two-sided gate | **Comparator RESOLVED; complete gate NOT RESOLVED** | Both overrun directions and omissions are checked (`084-03-lib.py:194–213`). Gap/suffix handling and missing-mask acceptance remain false-pass paths; see executable counterexamples below. |

**F1 — P1: “no PendingUpdate now” does not fence all work computed under the old hold.**

The existing-pending-operation case is improved correctly: `Restoring` retains a baseline-looking texture while an operation is pending (`Mip:996–1028`), and removes it only at baseline with no pending operation (`:1123–1133`). Freezing that fact before the frame renders avoids using a later game-thread reading to certify an earlier picture.

But UE has another queue before a texture gets its `PendingUpdate`. The streaming manager starts mip calculation in the background and applies its load requests in a later processing stage (`UE/Engine/Private/Streaming/StreamingManagerTexture.cpp:1679–1695,1699–1748,1362–1366`). There is also an explicit deferred mip-copy queue using cached wanted counts (`:1341–1356,1410–1444`). `StreamWantedMips_Internal` subsequently calls `StreamOut(LocalWantedMips)` when the asset currently has **no** pending initialization/streaming (`UE/Engine/Private/Streaming/StreamingTexture.cpp:550–584`). The predicate used by this fix only sees the asset's update/init state (`UE/Engine/Private/StreamableRenderAsset.cpp:229–262`). Changing the bias does not invalidate those queued plans: `UpdateCachedLODBias` only assigns the cached bias (`UE/Engine/Private/TextureDerivedData.cpp:2765–2768`).

A source-permitted sequence is: low-mip intent calculated under the hold; early revert while the resource still looks baseline and no asset update has started; `Mip:1004–1009` takes `AlreadyBack` and creates no restoring entry; two baseline arms/receipts qualify as settled; the queued old intent later dispatches its stream-out. A delayed calculation/copy queue can outlast those confirmations. Neither the new flag nor the closure fence acknowledges a post-revert streamer generation. This is a source counterexample, **not** a measured occurrence on this binary, and is distinct from temporal AA residue.

Proposed correction: make restoration retirement depend on acknowledgment that requests derived from the held bias can no longer be issued, then confirm baseline on subsequent matched renders. A can-fail case needs queued-but-not-yet-issued stream-out work, as well as the already-present `PendingUpdate` case. `Tests:283–324` supplies the settled booleans itself and proves only the latter state-machine behavior.

**F2 — P1: the cursor reopens, but the event's other products and lifecycle do not.**

The ordinary cross-batch baseline/held/baseline defect is repaired. An absent head still in `PendingSnapshots` ages out after 16 ticks; a terminal head is marked missing immediately. With normal ticking, a single lost request does not leave `ORDER HOLD` waiting forever (`Capture:6070–6111,6226–6237`). Shutdown has a bounded force/drain path (`:4595–4602`). Missing receipts break confirmations instead of counting as baseline.

However, a closure fence can be reached with newer armed receipts outstanding. The candidate's own test explicitly closes at 503 while 504 remains in flight, then reopens on held 504 (`Tests:406–418`). Insert a real GT service between those deliveries: `Capture:6241–6256` issues event end and releases the reservation. Frames armed after closure omit this trail from their mask/watch sets (`:1344–1351,5191–5197`).

When 504 then reopens the event, `HandleTrailReopens` appends the fire to pending snapshots, appends **mask value 0**, and removes cached render results (`Capture:6028–6043`). It does not recover an armed mask, update pending mask tag/event maps, or add the event to already-sampled `ObserveQueue` entries. Thus a frame can be relabelled unknown/positive while its target has no delivered mask and m55 has no corresponding label. A closed m55 event rejects later observations (`Change:82–83`); the reopen path has no m55 reopening operation. Counting already-written unwatched frames (`Capture:6045–6053`) diagnoses their loss but does not repair it.

There is also a deterministic cleanup defect: reopening never resets `bEndEventsIssued`, and setting `restore_reopened` does not set `bCarriedRefusal` (`Capture:5999–6059`). If the first closure was serviced, a later baseline closure skips the entire cleanup block at `:6241`. The target remains reserved and m52 remains refused for the rest of the run despite recovery.

Proposed correction: coordinate reopen eligibility across the snapshot, mask, m55 and reservation lifecycles. Delay irreversible detachment/finalization while a receipt can require reopening, or explicitly classify unrecoverable products as failed qualification. Reset/manage closure and refusal ownership on each reopen. The test should execute close → GT service → an arm without the event → late held/unknown receipt → reclose, checking all products and release state, not only `FTrail`.

**F4 — P1: the new-component set is not a component-registration or texture-binding monitor.**

The Apply census now has the requested static scope. `TEXUSERS` independently traverses `UActorComponent` with the same loaded-level scope, without a registration filter (`Capture:4760–4849`). The original inactive-level and unregistered-user omissions are repaired.

Two ordinary lifetime routes still escape:

1. An existing unregistered component does not use the held texture at Apply, so it is harmless and enters `HoldKnownComponents`. During the hold, `SetMaterial` changes it to use the held texture, then it registers. `UMeshComponent::SetMaterial` calls `MarkRenderStateDirty`, but that function broadcasts only for a registered component with a render state (`UE/Engine/Private/Components/MeshComponent.cpp:48–78`; `.../ActorComponent.cpp:1834–1843`). Registration creates the render state through `ExecuteRegisterEvents`, without that dirty broadcast (`.../ActorComponent.cpp:1349–1351,1670–1687`). The object identity is already known, so `Mip:1445–1449` never rechecks it. It can remain an unflagged foreign user for the whole hold.
2. An already registered foreign component changes an existing MID's texture parameter to the held texture. `UMaterialInstanceDynamic::SetTextureParameterValue` calls the material instance setter (`UE/Engine/Private/Materials/MaterialInstanceDynamic.cpp:221–234`). That setter updates the material render resource/uniform expressions directly (`.../MaterialInstance.cpp:3408–3436,390–414`); it does not mark the component's render state dirty. Neither the identity diff nor the subscribed delegate observes this new user.

`IsComponentStillLoading` does not fix either route. There are additional scope-transition weaknesses: startup seeds every non-loading component encountered globally as “already judged,” not just the census participants (`Mip:1273–1289`); `SettleHoldComponent` treats absence of a component level as settled (`:93–96`); dirty entries skipped while loading are removed from the queue (`:1473–1486`). These predicates cannot establish that the component's future ownership/material state was judged. Even detected dirty changes are queued until the next monitor tick, rather than reverted in the callback (`:1352–1359`). The monitor is also stopped at Revert while the texture may remain below baseline (`:953–954,1096–1099`), leaving later restore-tail users unmonitored.

Proposed correction: track registration/scope and actual texture-binding changes, including unregistered material assignment and MID parameter updates, through a point that precedes the relevant render. Keep contamination coverage through unresolved restoration where it can still affect other users. The existing `EvaluateNewHoldUser` unit test assumes detection has already happened (`Tests:542–573`); it cannot prove these engine routes.

Affordability is **unmeasured**. Each active-hold tick constructs two global class iterators and a loaded-level set; the known set retains identities until stop (`Mip:1434–1464`). UE's templated iterator builds an object array with `GetObjectsOfClass` (`UE/CoreUObject/Public/UObject/UObjectIterator.h:252–266`). Avoiding material enumeration for known objects helps, but this remains work proportional to global component population each tick. The Apply-time enumeration timer does not measure it. I found no basis to certify its cost on Concorde or Bates from these unit tests/builds.

**F5 — P1: inherited labels begin before inherited masks can be armed.**

The carried/restoring texture ownership mechanism is a substantive repair: `CarriedTrails` survives `ResetRenderTruth`, keys and fire identity survive for carried events, and adoption rebases SI state (`Capture:203,278–289,5891–5981`). Live-window cuts now open trails and are counted before reverting. On a supported new run, inherited membership reaches the normal label/annotation accumulation path (`:5191–5199,4530–4533,7700–7708`).

But each new run resets mask records (`Capture:3530`; `MaskMeasure:27–29`). `EnsureMaskRecordsForCapturedFrame` creates records **only for Auto's live fires**, not adopted trails (`Capture:1287–1302`). `ArmTargetMaskOwn` includes adopted trails but requires an existing record/tag (`:1344–1387`). There is none for the opening inherited frame. Its first record is created only when that frame later reaches `AccumulateFrameEvents` (`:4532,7732–7735`), after the corresponding render and mask opportunity. The first held frame, and potentially further frames armed before its readback, therefore gets an m52 label with an unmeasured/missing target mask. Annotation retains the injected SI but can omit it from the observable subset if later frames are measured (`:7878–7905`).

Proposed correction: create inherited/orphan mask records after the new ledger's `BeginRun` and before the first arm, and apply the deferred-onset/render-truth policy to those records. A stop/start test needs to assert frame-zero labels, PNG masks, m55 tags and annotation membership together. `Tests:421–444` tests only the pure trail rebase.

**F5 — P2: a non-render-truth intervening run drops the carried reservation/refusal.** `StartRun` deliberately retains `CarriedTrails` and applies their ownership for such a run (`Capture:3395–3402`). `FinishRun` clears all reservations/refusals unconditionally (`:7625–7628`) but reapplies carried ownership only when `bRenderTruthRun` is true (`:7636–7639`). End a non-SVE/legacy-timing run while the carry is unresolved: the carry array remains, but the promised persistent refusal/reservation does not. That run can also capture the still-blurred scene without attaching inherited labels, since adoption is restricted to render-truth runs. Proposed correction: preserve carried ownership independent of the intervening run's capture mode and explicitly resolve capture admission when inherited truth cannot be observed.

For event-key provenance, carried events retain their original key. Orphans use a synthetic future `StartFrame` (`Capture:5934–5944`), which is serialized as `start_frame` (`Writer:95`). I found no ordinary immediate key collision, but this is not the original Apply frame; it merits explicit inherited/synthetic provenance rather than being interpreted as measured onset metadata.

**F6 — P2: the hard-bound fallback is final only inside the m55 queue.**

The old forced-negative bug and the specific shutdown ordering are repaired. `ResolveObserve` waits or selects labelled unknown, and `FinishRun` flushes before `BeginClosure` when observations are queued. I also checked the unchanged `EndEvents` implementation: it can defer to the latest unobserved SI (`Change:127–132`), so an ordinary EndEvents call alone is not evidence that every outstanding observation is lost.

The remaining disagreement is at the hard bound. At age >64, `FlushObserveQueue` sets the queued m52 label true, submits it once and removes it (`Capture:6593–6624`; `Window:447–458`). It neither stores a canonical terminal unknown result nor cancels/terminalizes the corresponding pending snapshot. If a **live, non-trailing** frame's baseline receipt arrives later, `ComputeRenderMembership` uses `LiveMembership(NotHeld)` and the label/mask become out (`Capture:6357–6358,6412–6438,6471–6500`). m55's observation cannot be replaced (`Change:75–77`). Delaying colour completions beyond the bound supplies this case; unlike trailing frames, live frames do not have a cursor's immutable missing slot. Whether m55 still retains that pending item also affects whether the forced observation is accepted at all.

Proposed correction: make the fallback terminal in the shared per-frame authority before m55 consumes it, or explicitly reject the missing evidence while preventing a contradictory later membership. Test a late baseline after the hard bound, not just the helper's return value (`Tests:446–459`). Reopened event continuity is additionally unresolved as described under F2.

**Two-sided gate — P1: the comparator is repaired, but the oracle/censoring and mask checks can still pass invalid evidence.**

`gate_sync` now rejects early and late overlabels and missing visible frames inside the selected window (`084-03-lib.py:194–213`). `084-03-window.py:102–129,139–182` writes variant label files and mask PNGs referencing the banked image files, freezes the original oracle, requires the repaired case to pass the pixel/record/window checks, and requires all four doctored shifts to fail the pixel gate. This addresses the previous four-frame settle-tail counterexample. I inspected that artifact-writing path; I did not rerun the full dry runner or claim new banked-pixel evidence.

I executed the supplied 15 checks through AST-selected, unmodified functions: **15 passed, 0 failed**. Additional synthetic checks using those same measurement/gate functions produced:

| Input | Current result | Required interpretation |
|---|---|---|
| Visible and labelled 10–14; SI 15 missing; clean 16–18 available | `censored=false`, sync PASS, window PASS | Offset has an uncaptured gap; not a measured exact edge. |
| Visible and labelled 10–14; engine-frame jump at SI 16, inside the three-clean-frame suffix | `censored=false`, sync PASS, window PASS | The purported clean suffix crosses an unobserved interval. |
| Visible 10–14 **and 20–24**, labelled/masked only 10–14 | sync PASS, window PASS; 20–24 listed as “isolated” | A five-frame recurrence is not an isolated visible frame. It cannot silently qualify the original offset. |
| Visible/labelled/render-held 10–14; no masks, mask values 0, `target_pixels=-1` | sync PASS, window PASS | Mask synchronization was not established. This can conceal the inherited-mask defect. |

`measure_event` counts the next three **available** clean samples, then stops at the first such run (`084-03-lib.py:163–173`). Its gap checks require the immediately adjacent SI to exist, and inspect only the first suffix edge (`:174–176`). A missing adjacent row therefore disables the check instead of censoring. `gate_sync` calls every visible sample outside that selected window “isolated,” without testing whether it forms a sustained run (`:196–197`). `gate_window` requires masks only when the plugin reports `target_pixels>0` (`:214–220`); the validity check validates colour PNG counts, not target-mask coverage (`:458–470`).

The top-level frame-count check can reject a simple dropped-row leg before acceptance; that does not make the censoring result correct, and it does not reject engine-frame gaps with the requested number of captured rows.

Proposed correction: validate contiguous SI and engine-frame support at both boundaries and throughout the confirming clean suffix; classify absent neighbors/gaps as censored. Distinguish genuine isolated excursions from later visible runs and give recurrences an explicit failed/unmeasured disposition. For independently pixel-measurable target events, missing/unmeasured masks cannot produce a window PASS. Add these cases to the known-answer suite alongside the four shifts.

The purity gate also reports `PASS` when a valid Apply census is followed by a `HOLD CONTAMINATED` line: contamination is collected after verdict calculation and has no effect on it (`084-03-lib.py:240–261`). I reproduced that result. Reporting the line is useful and matches the README's description, but it does not certify lifetime purity or verify the required per-frame contamination flags. Chat should distinguish Apply-census PASS from lifetime purity before using this gate to qualify decision 3.

The executable script and results are [offline_checks.py](D:/IntrusiveAnomalies/_relay/runs/2026-09-28-084-02b-m52-minidelta/offline_checks.py) and [offline-checks.json](D:/IntrusiveAnomalies/_relay/runs/2026-09-28-084-02b-m52-minidelta/offline-checks.json). These are gate-logic demonstrations using synthetic sharpness values, not runtime or real-image measurements.

**Answers to the six implementation challenges**

- **(a) ORDER HOLD:** a single lost trailing request has a working terminal/age path; I found no permanent single-head wait with normal ticking. The entire remaining batch is held (`Capture:4300–4312`), and an m52-containing batch is sorted as a whole (`:4249–4257`), so mixed non-m52 output can be delayed/reordered with it. This is not a global ordering guarantee for every output stream. Long loss bursts/ring overflow still merit stress coverage; the 16-tick bound is per cursor head, not an end-to-end delivery bound.
- **(b) Settled arm flag:** useful and conservative for an already-issued asset operation, but not a generation fence against queued streamer intent; F1 remains open.
- **(c) ExtendRenderWatch:** I found no new data race between the three named operations. Extend, ConsumeWanted and RT Take use `StateCS` (`SveCap:25–46,161–176`). Consume removes pending eligibility before the family is published (`SVE:113–127`), so extension either precedes consumption or fails conservatively. GT snapshot metadata appended on failure yields missing samples/unknown, not an invented baseline. The inherited owner-family/key-ring caveat from 084-02 remains: owner rejection does not stop publication and RT watch consumption precedes the m55 claim (`SVE:87–127,172–193`). This delta does not qualify arbitrary multi-family/multi-view capture.
- **(d) Monitor:** static scope is improved, lifetime detection is unsound on the two ordinary routes above, and per-tick cost is unmeasured. Rejecting the unsafe create-listener approach does not make the replacement equivalent to registration/material-change coverage.
- **(e) Carry/orphan:** event attachment and SI rebasing work at the source-state level, but opening masks, observable annotation membership, non-render-truth boundaries and synthetic origin metadata prevent an unqualified pass.
- **(f) No m52:** for a fresh run with no m52 fire/attempt, no inherited restoration and no outstanding m52 queue, I found no intended change to label/mask/m55 decisions. Conditional sorting stays off (`Capture:4223–4248`), observations take the direct branch (`:5482–5496`), and additional render-truth summary fields remain conditional (`Writer:760–764`). No runtime byte-comparison was performed. “No new m52 fire” with inherited restoration is deliberately different and currently has the F5 defects; mixed runs can incur the batch delay above.

**Evidence limits and disposition.** The 93-check C++ result is supplied evidence; I inspected its source but did not compile or execute it. It covers the pure predicates and cursor substantially better than the previous suite. It does not instantiate the capture/mask/m55 lifecycle, monitor engine registration/material changes, or establish that supplied settled flags describe all streamer queues. A successful Development build and the supplied dry run cannot close those integration findings.

**Chat review REQUIRED** for disposition of this review and the corrected source/gate qualification sequence, because 084-02 decision 1 requires correction before the bench. No implementation or bench action is issued here. The next mandatory checkpoint is chat Claude's ruling on these findings; eventual pixel-edge evidence must still satisfy the 085-00 ADDENDUM. The zero-tail residual experiment remains appropriate after the independent correctness blockers are resolved or explicitly ruled on.

VERDICT: CHANGES-REQUIRED
