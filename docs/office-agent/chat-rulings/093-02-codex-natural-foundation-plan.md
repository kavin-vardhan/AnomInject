<!-- Collected by Claude Code (brief 093-02) from relay run folder D:\IntrusiveAnomalies\_relay\runs\2026-10-13-093-02-natural-foundation-plan | model=gpt-6-astra effort=max (run-meta.txt) | duration_s=2741 (14:37:43-15:23:25) | content below is review.md unedited -->
# 093-02 — Natural-cause foundation and MISS pilots: proposed implementation plan

2026-10-13 · Independent reviewer · **Planning only; Chat review REQUIRED and owner go/no-go REQUIRED before implementation.** This report proposes work; it does not dispatch it or authorize a build. The requested ruling, feasibility review, Code verification, draft, compatibility principle, follow-up rulings/backup, and release checklist/readme were read in that order. Ruling 093-01 and Code's upheld corrections control this plan.

## One-screen summary

- **Base:** a new integration branch descended from frozen `release/2026-10-05` **`d7eee97`**, selectively incorporating all six 091-05 implementation commits, then completing the saved 091-06 F1 work. Preserve the release's **0/0/0** temporal defaults. Do not base on `master`, import VT branches, or change the delivered branch/binaries.
- **Identity: recommend (a), office-robust F1.** The saved patch addresses a demonstrated delayed-viewport outage, but is unfinished and its scene-plus-game-view fallback is insufficient for arbitrary multiple game views. Finish request-frozen ownership, ambiguity rejection, accounting and proof. The office's exact failure mechanism remains unconfirmed.
- **Foundation:** a provisional attempt inside the existing scheduler; clean census/baseline → arm → request-keyed RT mechanism + mask + picture evidence → count or revert → measured recovery. One cause owns the interval through recovery. No-effect attempts create no events; uncertain frames are excluded before publication.
- **Pilots:** `missing_texture` remains the type ID. **MISS-1 = controlled engine-default-material calibration**, never shader failure. **MISS-2 = natural DefaultTexture assignment to a qualified colour parameter through a private MID.** VT bindings are skipped; stable host MIDs are supported through sibling instances, not MID parenting.
- **Labels:** only the R8 additive natural key families; legacy frame lists, IDs, pixel meanings and m26 remain. Optional provenance never decides admission. A publication barrier is necessary: today's m55 receives colour after the writer has already written the label.
- **Delivery plan:** **11 serial implementer jobs, about 40–62 engineering hours**, each with one bounded home bench session. Two office checkpoints: F1 identity after J2, pilots after J11. Owner supplies only the numeric forms in §F; zero eligibility is not a supported-host pass.
- **Approval choices:** integration/identity design; bounded pilot measurement scope and experiment settings; UI A or B. **No new module dependency proposed.** Runtime success and office yield are unproven by this read-only plan.

## A. Integration base and owner-view identity

### A1. Source/evidence coordinates and live state

Paths below use `P = E:/IA_BuildCache/_r917/AnomalyInjector`, `C = P/Source/AnomalyCapture`, `I = P/Source/AnomalyInjector`, `A = I/Private/Anomalies`, `E = D:/UESource/UnrealEngine/Engine/Source/Runtime/Engine`, and `R = D:/IntrusiveAnomalies/_reviews`. Existing-source line references are to release `d7eee97` unless a git object is stated. **NEW** marks proposed files. File pairs in tables list both files and their respective interface/implementation work.

Read-only git inspection resolved release HEAD to `d7eee97731812384d4fd00af9032254923363ada`; status reported no changes. The named follow-up objects exist. Commands used `--no-optional-locks`; after ownership rejection, git reads used command-local `-c safe.directory=*`. No checkout, index write or git configuration change was made. The user-level ignore file was inaccessible; that warning was not treated as evidence about runtime behavior.

The extra retained evidence that materially changes the assessment is the 091-06 run's `brief.txt` and `events.jsonl`, under `D:/IntrusiveAnomalies/_relay/runs/2026-10-04-091-06-ownership-office/`. There is no final `report.md` or office readings file there. Its transcript records old-build selected-viewport PIE and two-client PIE passing with 912 frames each, then a delayed-viewport control yielding **912 dropped requests and zero images**. These are retained implementer readings, not tests repeated here; the deleted workspace is not available as fresh proof.

### A2. Proposed integration, preserving the delivered behavior

Start a future isolated integration branch at `d7eee97`; proposed name `feat/natural-foundation`. Apply the following **non-merge implementation commits in dependency order**, resolving against the release, not replacing the tree wholesale with `5d6352b`:

| Commit | What comes in | Files/behavior requiring review |
|---|---|---|
| `6415b62` | F1 ownership/family identity and F6 event-identity verification | Capture ownership, colour/mask SVE, family data and key ring; capture/label accounting; ownership/mask tests; `tools/verify_capture.py`, `tools/test_annotation_transitions.py` |
| `a15b24d` | F2 positive changed-component scope; F3 unused-alpha correction | `A/Anomaly_TexCorrupt.{h,cpp}`, `A/TexCorruptTree.cpp`, `A/TexCorruptPure.h`; uniformity tests |
| `4c82ee2` | Return excluded components' owned stencil tags | `C/Private/AnomalyStencilTag.cpp`, `I/Public/AnomalyLabelSync.h`; scope test |
| `45eba38` | Compile correction for the stencil change | `C/Private/AnomalyStencilTag.cpp` |
| `03e7eeb` | Dropped captures excluded from mask totals | Capture subsystem and label summary telemetry |
| `97c20f2` | Drain dropped mask results by session identity | Capture subsystem accounting |

`1eaf205` and `4c93e5f` carry the proof/documentation closure; `5d6352b` is the corresponding proxy merge. Bring their relevant explanatory docs and test expectations forward deliberately. Do not cherry-pick those merge commits on top of the six changes. The candidate must retain release proxy blur, UV and normal support; starting directly on `1eaf205` would not represent that complete feature base. `master b5f15a3` is context, not the proposed base.

**Expected conflicts:** the release changed `AnomalyLabelSync.h` defaults to 0/0/0, corresponding console help in `AnomalyCaptureSubsystem.cpp`, transition tests and readme. The raw `d7eee97..5d6352b` diff reverses those defaults to **3/40/1**; that reversal is specifically unwanted. Preserve `23098fa` behavior and `d7eee97` release history, including explicit-cvar override tests. Expect overlapping subsystem hunks and documentation/status conflicts. The saved 091-06 patch is based on `1eaf205`, so port its intent onto the combined tree rather than assuming an unreviewed patch application is the integration. The backup `r913-staged.patch` mostly reverts fixes: it is **not** an input.

Re-prove F1 across owner modes, F2's C1/C2 mask scope and tag retirement, F3's opaque-alpha no-effect/masked-alpha positive pair, and F6's wrong event ID without transitions. Re-run 091-05's dropped-mask accounting and m52/m53 regression with **0/0/0**, not its former 3/40/1 baseline. Old banked passes establish the mechanisms and can-fails, not this new combination. F4 selection heuristics and F5 copy-snapshot robustness remain documented limitations of the controlled copy routes; MISS-2 must not acquire those copy-specific restrictions or advertise them fixed.

### A3. Why F1 can fail closed, and what the backup actually does

At `5d6352b`, `AnomalyCaptureOwnership.h:5–7` requires non-null owner scene **and** target and exact equality. `AnomalySceneViewExtension.cpp:65–79` tests that predicate during activation using `Context.GetWorld()` and `Context.Viewport`, before a family is available. `EnsureCapturer` binds `World->Scene` and `World->GetGameViewport()->Viewport`. A null/stale owner target can remain installed after the viewport appears. A context constructed from a scene has no viewport; a viewport's client can also return no world. UE's `SceneViewExtensionContext.h:28–59` explicitly supports both forms; `UnrealClient.h:874` has a null default `GetWorld()`. Ordinary `UGameViewportClient` overrides it (`GameViewportClient.h:146`, `.cpp:584`), so “all PIE contexts are null” would be false.

The retained delayed-viewport control proves one complete-outage path. The actual office report only says no frames on `5d6352b`, while the earlier build captured. It does **not** identify null context, late viewport, multiplayer, HDR or target mismatch as the office cause. That distinction must survive the implementation report.

The saved `R/2026-10-10-disk-cleanup/r916-091-06-uncommitted-wip.patch` changes nine tracked files and needs the untracked `Source/AnomalyCapture/Private/AnomalyCaptureFamily.h`:

| Saved work | Assessment and required completion |
|---|---|
| `AnomalyCaptureOwnership.h`, new `AnomalyCaptureFamily.h` | Adds rejection reasons, counters, deferred activation and a game-family predicate. It permits a non-null family target when owner target is unknown, provided scene/game flags match. This is a plausible recovery route, **not unique owner proof**. Two game views in one scene can satisfy it. |
| `AnomalyCaptureSubsystem.cpp`, public header | Adds `RefreshCaptureOwner` at start/tick/world-tick-end, using world/local-player viewport clients; warning/state fields. This addresses the demonstrated stale-null binding. It chooses the first matching local player, without a declared uniqueness check or request ownership generation. |
| `AnomalySceneViewExtension.cpp`, `AnomalySveCapturer.{h,cpp}` | Activation defers the ownership decision; family checks remain before dequeue and after-pass. The new owner values remain mutable global capturer state. Pending requests need frozen ownership and generation checks. |
| `AnomalyMaskSceneViewExtension.{h,cpp}`, `AnomalyChangeFamilyData.cpp` | Uses the same game-family test for masks/m55, records counters. Colour, masks and m55 must all bind to the **same chosen family**, not independently accept two plausible families. m55 still requires one view in a family. |
| Diagnostics/proof | The warning says to read `capture_ownership` in `run_summary.json`, but the saved patch has **no label-writer/summary serialization change**, no amended ownership tests and no final proof. Do not call the backup a finished fix. |

**Recommendation (a): complete this work**, with these boundaries:

1. Activation only decides whether the extension may participate. Unknown activation context is not permission to consume a request. Resolve the display's viewport from the capture world's local players on GT at run start and before issue; reject ambiguous display selection. Refresh on creation/change.
2. Freeze `(run_epoch, owner_generation, scene, target, local_player/view identity)` into each issued colour, mask, census and mechanism request. Before dequeue, bind a unique `family_id` and `view_index`; at RT require that binding again. A viewport change invalidates pending requests and baselines; it never retrofits a new owner onto old work.
3. Initially require a resolved owner target for certifying natural observations. The WIP's target-unknown/game-flag fallback may be retained **only after** a bounded discovery step proves one eligible game view for that world and binds its actual target for subsequent requests. A first-arriving game family alone is not proof of uniqueness. If that cannot be established through public interfaces, defer capture with a numeric reason. Do not add private renderer access to guess.
4. Single-view families are the pilot measurement scope. Separate multiplayer PIE worlds may each resolve their correct display; split-screen/stereo or multiple same-world game displays remain explicitly unqualified until view-index/rect support is proven. Count refusals, do not capture a plausible editor view.
5. Keep F1 family IDs, same-sample expiry and dropped-result accounting. Add end-of-run numeric ownership counters and a local numeric reader. For this plan the F1 counters can be logged/read locally; **do not invent a `capture_ownership` summary block outside the R8 schema proposal**. Correct the WIP warning to point to the actual numeric report. A separate F1 summary block, if wanted, is a chat schema decision.

**Risks of (a):** broader capture changes; legitimate view arrangements may still be refused; owner changes during queued work; fallback ambiguity; missing counters mistaken for zeros. It needs J2's positive and negative proof and office checkpoint I before a supported-office claim.

**Alternative (b):** retain release frame-number keying, selectively take F2/F3/F6, and restrict captures to the readme's **one live game view**, new-window PIE with other realtime editor views paused/closed, or packaged/standalone. Advantages: matches the office-working delivery and avoids unfinished F1. Risks: the old build demonstrably consumed editor-view pictures in 091-05, family frame numbers can alias, the operational limitation is not enforced, and m55's separate owner predicate can still refuse measurement. Counts of written frames alone cannot prove the picture's origin. This is a possible owner-limited product choice under R11, but a weaker foundation. I do not recommend it or silently fall back to it after (a) fails. §F gives the exact numeric proof forms for the recommended choice.

## B. Foundation, file by file

### B1. Attempt lifecycle and scheduler integration

The existing apply path records successful `IAnomaly::Apply` immediately (`I/Private/AnomalyInjectorSubsystem.cpp:805–899`); auto then creates a live fire and reports yield (`AnomalyAutoInjectorSubsystem.cpp:590–619`). That is unsuitable as a natural-success counter. Add a provisional attempt, separate from the public effect event, inside those existing paths. Manual targeted pilot requests enter the same coordinator; they do not bypass measurement.

| Stage | Proposed behavior and terminal conditions |
|---|---|
| **Select / baseline** | At the existing auto cadence, take one candidate from a **clean, current-owner census**. Rank screen coverage/useful colour signal; retain the seed and selection denominator. Acquire one cause lease, verify no controlled fire or restore tail is live, and obtain paired clean target/control samples and RT baseline. Freeze component, mesh, slot/parameter and resource identities. Stage a private identity MID where needed and prove unchanged appearance before changing its parameter. |
| **Arm** | Record the internal attempt ID and exact owned restore snapshot before the setter. Queue material/uniform updates, retaining referenced UObjects/resources. The cause's arm time is diagnostic intent, not an onset label. Revalidate identity/clean baseline just before mutation. |
| **Observe** | Request colour, original-object mask and RT mechanism state under one key. Provisional tags/scopes exist before an event is counted. Evaluate the aimed object and every discovered affected visible user. Do not await a fixed one-frame GT delay or use `WasRecentlyRendered` as receipt. |
| **Count / no effect / uncertain** | A measured aimed effect commits one success, and one legacy effect event per affected visible object. A clean-noise no-effect result reverts and counts only as an attempted no-effect. Above-noise/below-visible or missing evidence reverts as ambiguous/unknown and excludes those source samples. A missed aim with visible collateral commits the collateral events and records missed aim, never aimed success. |
| **Revert** | On hold expiry, no-effect, uncertain result, stop, lost target or failure, execute the owned restore recipe. Keep observation and the scheduler lease live. Request cancellation does not release UObjects still referenced by RT/readback work. |
| **Recovery** | Verify raw state, RT material/uniform state and picture recovery with the same target identity. Require clean-noise recovery samples; dispose of private MIDs after render retirement. Only then invalidate contaminated census entries, resume clean census and release the lease for the next attempt. Lost ownership or an unverified restore stops re-arming, with a counted reason; no timeout silently declares a clean world. |

A cause lease covers **baseline through recovery**, not just the knob-on period. It also blocks ordinary auto/manual effects that would spoil attribution; configuration may be queued for the next attempt, while revert/stop remains available. This extends the m52/m53 exclusion concept without adopting its “admit after unresolved restore” exception for natural work. The existing interval/hold controls remain the rate controls; hold starts from observed onset. There is no wait for a naturally occurring accident and no second scheduler. Unsuccessful candidates are fairly rotated after recovery; backoff is for a specific unchanged candidate, not a permanent material-name blacklist. Record attempts/minute and successes/minute; no office frequency promise follows from source alone.

Census currently stores actor, mask count and `MeasuredAtTick`, with a freshness window and bounds fallback (`C/Private/AnomalyCensus.h:24–36`, `.cpp:287–343`). Add owner/cause cleanliness epochs and component/mesh/material signatures. Natural selection accepts only a fresh measured nonzero entry from the current clean epoch; bounds fallback may help enumeration, **never natural eligibility**. Revalidate the chosen component with a new clean sample even if the rolling census is fresh. The existing floor/ceiling are selection preferences, not claims of impossibility. During a cause, measurements may feed observations but may not overwrite the clean selection baseline. After recovery, measure again rather than reusing contaminated entries. Keep the bounded census and its 15,000-actor timing gate.

### B2. Proposed files and responsibilities

| File(s) | Proposed change |
|---|---|
| **NEW** `I/Public/AnomalyCauseTypes.h` | POD/value contracts for attempt handle, mode/recipe, stage/outcome, immutable target/binding identity, probe request/result and typed metrics. No capture-private includes and no Json dependency in Injector. Internal attempt identity exists even if optional cause metadata is disabled. |
| **NEW** `I/Public/AnomalyCauseProvider.h`, `I/Private/AnomalyCauseProvider.cpp` | Registered capture-provider callbacks for prepare/observe/recovery/results, following the census-provider dependency direction. Missing measurement provider makes a natural request unavailable; it does not reroute to controlled. |
| **NEW** `I/Private/AnomalyNaturalAttempt.h`, `.cpp` | One active cause transaction per world/capture owner; lifecycle, budgets, exact target identity, lease, outcome partition and recovery ownership. Interfaces in the header; state transitions and counters in the implementation. |
| `I/Public/IAnomaly.h` | Small optional prepare/arm/restore/watch interfaces or an adapter to the new cause transaction. Preserve ordinary `Apply` behavior for shipped recipes. A provisional arm is not exposed as a counted live event. |
| `I/Public/AnomalyInjectorSubsystem.h`, `I/Private/AnomalyInjectorSubsystem.cpp` | Mode/recipe dispatch under unchanged catalog IDs; shared transaction entry for console/targeted/auto; reject competing causes; commit effect records only after accepted results; keep stop/teardown restore active. |
| `I/Public/AnomalyAutoInjectorSubsystem.h`, `I/Private/AnomalyAutoInjectorSubsystem.cpp` | Provisional vs committed fire status; natural candidate requirements, deterministic rotation and separate yield counters; lease-aware `ServiceReverts`, hold timing and `RevertAllLiveFires`. `MaxConcurrent` does not allow overlap with a natural attempt. |
| `I/Public/AnomalyCensusProvider.h`, `I/Private/AnomalyCensusProvider.cpp` | Extend opinions with cleanliness/owner identity and measured footprint information; preserve old consumers' fallback behavior. |
| `C/Private/AnomalyCensus.h`, `.cpp` | Stamp every entry/batch with clean epoch and owner identity, suppress dirty promotion, reject stale completions, refresh after recovery and record natural-selection denominators. |
| `C/Public/AnomalyCaptureSubsystem.h`, `C/Private/AnomalyCaptureSubsystem.cpp` | Register provider, issue provisional mask/probe watches, join same-request evidence, route accepted/uncertain results, await recovery and drain safely on stop. Keep owned samples until their publication outcome resolves. |
| **NEW** `C/Private/AnomalyMechanismProbe.h`, `.cpp` | Capture-request RT readers for MISS-1/MISS-2, typed extension points for residency and cull state, explicit unknown/error results, immutable generations and retained resources. |
| `C/Private/AnomalySveCapturer.h`, `.cpp`; `AnomalySceneViewExtension.cpp` | Carry probe request arrays alongside the existing render-texture watches; sample them at the owner colour callback, return the same request/family/view key. Keep the existing m52 watch path working. |
| `C/Private/AnomalyChangeFamilyData.h`, `.cpp`; `AnomalySveKeyRing.h`, `.cpp`; ownership/family and mask-SVE files in §A | One immutable family binding shared by colour, mechanism, mask and census. A duplicated callback, stale epoch or wrong view cannot finish the request. |
| `C/Private/AnomalyMaskMeasure.h`, `.cpp`; `AnomalyStencilTag.h`, `.cpp` | Give provisional attempts owned tags/scopes without manufacturing events. Measure target support on every retained pilot sample, keep baseline-only measurements separate from m26 effect accumulation, and promote the same identity on success. |
| `I/Public/AnomalyViewport.h`, `I/Private/AnomalyViewport.cpp` | Explicit changed-component scope throughout install and recovery, using the F2 semantics; no whole-actor fallback when scope is empty. Separate candidate geometry from an effect measurement. |
| `C/Private/AnomalyChangeStage.h`, `.cpp`; `AnomalyChangeMeasurement.cpp` | An attempt channel with clean controls, identity-stage checks, pre-arm reference, all effect/recovery samples and three-way classification. Existing post-label m55 rows keep their meaning; they are not renamed into proof retroactively. |
| **NEW** `C/Private/AnomalyAttemptPublisher.h`, `.cpp`; `C/Private/AnomalyAsyncWriter.h`, `.cpp` | Bounded prepare/commit path described below: canonical pixels and mask acknowledgements before final labels/event accumulation. Distinct sampled vs published indices; source-order publication and coordinated image/mask/label failure handling. |
| `C/Private/AnomalyLabelWriter.h`, `.cpp`; `C/Private/AnomalyActiveSource.h`; `I/Public/AnomalyLabelSync.h` | Explicit per-event pilot evidence policy, preserving ID-based legacy paths; serialize only §B5's additions; consume finalized samples for annotation and JSONL. A natural `missing_texture` must not accidentally inherit the ordinary fire-window-only policy just because its ID is unchanged. |
| `I/Public/AnomalyTelemetry.h`; `C/Private/AnomalyRunLog.h`, `.cpp` | Value-form cause counters, stage diagnostics and measurement/restore results; aggregate without counting an arm as success. No UObject access from the writer. |
| Console/protocol/dashboard, tests, docs and fixtures | Exact files and responsibilities in §B6, §E and §F. No shader or host-game module edit is proposed for these two pilots. |

### B3. Render-thread probes and future adapters

The immutable join key is `(run_epoch, owner_generation, attempt_id, capture_request_id, sample_game_frame, sample_ordinal, scene, target, family_id, view_index, rect/extent)` plus component/mesh/material/resource generations. The family fields bind before serving, not at completion. Colour/mask/probe equality is checked in the reducer; a late result cannot attach to the newest tick. Retain referenced materials/textures on GT through a GC-aware owner; RT reads public render state and returns value records, not mutable UObject names. Loss/recreation/feature-level changes yield unknown and a new baseline, not recycled pointer equality.

- **MISS-1:** read the watched material's `FMaterialRenderProxy`, its RT material/fallback state and compiled texture-uniform resources, with the GT installed-slot generation and ordered proxy-recreation receipt. `UMaterialInterface::GetRenderProxy` and public RT `GetMaterialNoFallback`/`GetFallback` are available (`E/Classes/Materials/MaterialInterface.h:405`, `E/Public/MaterialShared.h:2592–2602`). The default's own uniforms can contain several textures. An explicit default binding is expected; an unrelated unexpected fallback or lost slot identity invalidates the observation.
- **MISS-2:** use the effective material's RT uniform-expression set and `GetTextureValue(Type,index,render_context,material,out)` for **every compiled occurrence of the full parameter identity**, not only the UObject getter. Confirm expected DefaultTexture identity, live resource/reference/sampler and unchanged non-target bindings. Uniform filling checks resources and may bind white (`E/Private/Materials/MaterialUniformExpressions.cpp:910–1000`); pointer equality with a texture that has no resource is insufficient. A shader-map change invalidates saved uniform indices; rediscover by full identity or revert.
- These are **mechanism-state observations**, not exact per-draw/permutation receipts. Use passive no-fallback material/RT uniform state plus the m53 required-draw-shader readiness evidence, not a global complete-shader-map veto. `GetMaterialWithFallback` can differ from a pass's partial-map choice and can submit compile jobs (`MaterialShared.cpp:4207–4234`); it is not the per-sample visibility oracle or an excuse to trigger host compilation. Record missing/fallback state without claiming a draw receipt, and pair it with the captured mask and picture. Do not reach into new private mesh-draw internals to claim stronger telemetry than R4.
- **BLUR-1 later:** another probe payload reads coherent pre-arm and current `FStreamableTextureResource::GetState().NumResidentLODs`, first mip and resource generation. Provider returns a texture→binding→visible-user graph, not today's one-primary-owner watch. Every visibly affected user gets its own effect and recovery window. Restore may use only the R6 bounded `StreamIn` repair up to pre-arm state. No shared/private-copy policy is changed now.
- **GONE-1 later:** payload reads effective cull state from the recreated public proxy and the saved component generation; mask provider accepts a proven equivalent reference surface. Reserve `mask_measurement.method="equivalent_reference_surface"`; never supply RT1 from its draw count. R5 aiming includes bounds-origin distance, FOV scale and a fade margin only for DistanceCullFade. Proposed future equivalence experiment: IoU ≥0.995 and symmetric-difference area ≤0.5% of the union, across paired samples, plus unchanged colour within clean noise; also inspect thin-feature loss. These are hypotheses to qualify then, not permission to implement GONE-1 or use cached masks now. Offset/scale mutation must fail. RD restore order remains flags → saved LD → saved cached distance → proxy/reappearance proof.

### B4. Picture classification and publication

**Existing gap:** m55 measures only the first four already-labelled positions (`AnomalyChangeMeasurement.cpp:184–283`) and its colour callback arrives from `AnomalyAsyncWriter.cpp` **after** `EncodeAndWriteFrame`. `ProcessCompletedFrames` builds labels/accumulates events before enqueue (`AnomalyCaptureSubsystem.cpp:4947–4988`). Simply calling m55 from `Apply`, or deleting an event at run end, cannot implement R7 safely.

Proposed experiment settings, frozen in the predictions before the first leg:

- Five complete clean samples give four clean-to-clean pairs, followed by an unchanged private-MID identity interval where applicable. Keep the last clean reference and statistics; do not retain five redundant full images indefinitely. View/geometry changes require a new baseline; unsupported motion is a measurement limit, not evidence of no effect.
- For target/control footprints use m55's per-pixel `max(|ΔR|,|ΔG|,|ΔB|)` (`AnomalyChangeMeasurement.cpp:22–25`). Express the proposed decision values in **8-bit code values**. Existing m55 `*_mean` divides by 255; it must not be compared directly to a threshold of 4.0.
- Let `N_T` be the maximum matched clean-pair target mean plus 0.05, and `N_C` the analogous control bound. Proposed visible threshold `V_T = max(4.0, 4*N_T)`. Store the measurements, threshold and units. Also retain changed-pixel histogram/fraction; the independent house checker still runs its declared bbox/clean-null reading. Thresholds are bench hypotheses, not already accepted universal perceptual thresholds.
- With valid mechanism and nonzero same-frame mask: `D_T <= N_T` is a no-effect sample; `N_T < D_T < V_T` is ambiguous; `D_T >= V_T` is a candidate visible sample. Missing receipt, invalid resources/identity, excessive clean motion, or control disturbance not explained by measured collateral is unknown. Do not subtract arbitrary background change to manufacture a pass.
- Examine up to **four paired post-arm samples** for direct swaps, with a separate initial **2 s wall watchdog** for missing render/readback progress. A positive stable confirmation may require two qualifying samples, but membership is assigned back to each original qualifying source sample, not to decision time. A no-effect outcome requires complete samples within noise and no observed collateral; a missing sample is unknown. Do not keep extending an ambiguous attempt until one noisy frame passes.
- Continue the check on **every candidate labelled frame** and through restoration, not only onset. Two complete recovered samples within the clean bound are an initial recovery setting. A strong first frame followed by an uncertain one yields a gap/exclusion, not a filled interval. TAA/TSR residuals after material restoration are excluded until recovery; do not lengthen `injected_frames` to pretend the restored material is still applied. BLUR later can remain applied while its measured resource deficit persists after the knob resets.

**Publication choice: exclusion, not an invented positive or a clean-looking orphan transition.** For runs using a pilot, the publisher holds provisional source samples before constructing final JSONL/annotation entries. It can emit proven clean samples, emit measured effect samples, or exclude uncertain samples. All non-visible applied samples of an ultimately successful attempt are also excluded in v1; this avoids calling an applied-but-unqualified interval clean or changing legacy membership semantics. A wholly no-effect attempt can release only samples actually proved within noise; unknown samples never take that path. Recovery/control samples become clean only after recovery is proved. A visible collateral sample is committed with its event if measurable; unavailable collateral evidence quarantines that sample and increments failure diagnostics instead of silently erasing the effect.

Source sample ordinals and published `session_index` must be distinct. Allocate the published index only when the source-order decision is terminal; excluded samples receive no client frame index. This preserves **frame N = image N = mask-map N = JSONL N = video N** and keeps authoritative frame lists meaningful. Remap m55's published references, annotation lists and mask paths consistently, while retaining request/sample keys in `effect_observation` and diagnostic records. No schema-3 frame-map keys are imported. Exclusions may shorten a clip; the numeric report records excluded-sample counts and measured duration. This is explicitly a behavior requiring chat/owner approval, not a reinterpretation of the release's old indices.

Canonical colour conversion must happen before the decision, with exactly the conversion used for the written PNG. Successful mask measurement and **mask-file write acknowledgement** are required before a labelled frame is published. Prepare image/mask files privately, then commit the row and event accumulation only after required writes succeed. Failure removes/unpublishes the prepared bundle and accounts for it once; neither annotation nor JSONL may retain a positive whose image/mask failed. Final run summary follows closure. The existing all-controlled path is preserved unless a specific shared fix is proven necessary.

Use one accounted memory budget for retained colour, masks, raw buffers and outstanding evidence, initially reusing `IAI.Capture.ChangeMaxBytes` (release default 256 MiB, `AnomalyChangeStage.cpp:25`), plus a maximum of 16 outstanding source samples; whichever bound is reached first controls admission. The pilot path expands the accounting, not the controlled path's default allocation. Reserve the minimum baseline/observation capacity before arm. Oversize/native-resolution shortages defer without changing host resolution; unexpected exhaustion reverts and quarantines. These are explicit resource bounds under rule 3, to be measured at 1080p and the office resolution. No unbounded sidecar spool and no extra GPU flush per sample.

### B5. Label writer, summary and concrete examples

The following is the proposed **complete natural addition surface**, matching R8. Nested shapes specify the contents of those approved containers; there is no separate `effect_target`, `effect_mask`, `injection_id`, `frame_observations` or schema-3 migration. Keep annotation `label_schema:2`, `label_schema_minor:1` and existing summary/schema-version meanings. Preserve the existing `blink` annotation / `blinking` JSONL spelling difference and the existing event-tag range; new provenance does not rename either.

| Allowed addition | Placement and meaning |
|---|---|
| `natural_metadata_version` | Annotation root and run summary, integer `1`. It versions these additions, not the legacy label contract. |
| `injection_mode` | Each event and matching JSONL anomaly entry: `controlled` or `natural`, frozen at attempt selection. Existing recipes are controlled. Old files without it are legacy/unspecified, never inferred natural. Also carried inside a cause record so the container name cannot imply a mode. |
| `cause_id`, `cause_family`, `cause_event_id` | Optional event/entry provenance; IDs such as MISS-2, family `content_error`, unique per-attempt link. Event IDs keep the existing `type@start_engine_frame|target` form (`AnomalyLabelWriter.cpp:735–738`). One cause may link multiple event IDs. |
| Root `natural_causes[]` | Records only for attempts that emitted at least one observed effect. Fields inside: the cause/mode identity, truthful mechanism description, before/applied values, aimed target, source request keys for arm/revert/recovery, linked effect event IDs and restore result. No-effect/ambiguous-only attempts have no root cause record. Proposed convention: this envelope may also contain an effect-emitting MISS-1 calibration record, explicitly `controlled`; membership in this historically named container never asserts natural mode. |
| `effect_observation` | Per-frame state/method, request/sample/owner/family key, component and binding identity, material/uniform/resource validity, measured mask support, target/control/reference metrics and thresholds. Event form gives method, coverage and first/last observed source keys. Unknown evidence is null/absent with a reason in diagnostics, never fabricated zero. Do not serialize raw pointer addresses as durable IDs. |
| `severity` | Typed metric with explicit unit and scope. Pilots use `mean_max_rgb_delta`, unit `code_value_8bit`; frame value versus clean reference, event min/max/mean over observed affected frames. This measures appearance difference, not a universal realism score. Later mip deficit has its own metric/unit. |
| `mask_measurement.method` | `original_custom_depth` for these pilots. Reserve `equivalent_reference_surface` for qualified GONE-1. No companion counts enter `target_drawn_pixels`. |
| `cause_*` counts in `run_summary` | Enumerated below; all attempts and failures remain visible in denominators. Do not rename/reinterpret `positive_frames`, `bursts_done`, `vetoed_events` or mask counters. |

**Legacy semantics:** `injected_frames` remains applied membership on **published** frames; `affected_frames` is its measured-visible subset. The pilot publisher excludes unresolved applied samples rather than using an affected-only list to redefine all sampled cause-on time. The full cause-on interval lives in the cause/attempt diagnostics, not in a silently expanded injected window. Frame lists can have gaps, and counts are counts, not spans. `observable` retains the existing labelled/condition/mask-threshold rule; pilot label membership is decided upstream by evidence. `target_pixels`, `target_drawn_pixels`, `bbox_drawn_px`, mask provenance and `mask.provided` remain actual measurements. The pilot requires nonzero support **and** the configured `observable_min_pixels` for a counted observable success. It does not substitute a new hardcoded visibility pixel threshold.

Optional metadata is attached **after** the decision. Turning it off, failing to resolve a descriptive cause name, or omitting severity aggregation must not change admission, picture, mask or frame membership. The internal mechanism/mask/picture evidence is mandatory regardless of serialization. Normal successes retain it in the local evidence path even when cause annotations are suppressed. The checker should detect incorrect supplied provenance, but its safe resolution is to correct or omit that optional provenance bundle; absence of cause metadata is **not a fire, success or release gate**. Mandatory picture/mask/legacy-label consistency is a different requirement.

Proposed `cause_*` counters:

- Selection denominators: `cause_selection_rounds`, `cause_candidates`, `cause_eligible_candidates`, `cause_prearm_refusals` (with `cause_refusal_counts` by reason). A candidate rejection is not an armed attempt.
- Lifecycle: `cause_attempts`, `cause_armed_attempts`, `cause_successes_aimed`, `cause_no_effect`, `cause_ambiguous`, `cause_unknown`, `cause_missed_aim`, `cause_cancelled`, `cause_pending`.
- Effects/failures: `cause_effect_events`, `cause_collateral_effect_events`, `cause_requested_frames`, `cause_excluded_frames`, `cause_failed_frames`, `cause_probe_timeouts`, `cause_lost_ownership`, `cause_restore_unverified`, `cause_counted_without_mask`, `cause_stale_receipts`, `cause_wrong_owner_receipts`; `cause_counts_by_id` partitions lifecycle counts by cause and mode. Requested/excluded/failed frame counts cover the pilot publication path's source samples; written frames remain the existing `total_frames`. Existing frame/mask/write counters still report their original totals.

Define an attempt when the selected candidate enters baseline preparation; count its arm separately. At final closure:

`cause_attempts = successes_aimed + no_effect + ambiguous + unknown + missed_aim + cancelled + pending`.

Terminal outcomes are disjoint. Visible collateral without a verified aimed hit is `missed_aim` (diagnostics retain whether the aim was no-effect or unknown). Timeout/lost-ownership/restore counters are overlapping diagnostics, not additional terms. Pending must be zero in a completed certifying run. Count each aimed success once, each emitted object event once, and never count each positive frame as another successful cause. These lifecycle denominators cover natural attempts and MISS-1 calibration attempts; merely adding optional metadata to an ordinary shipped fire does not pretend it used this lifecycle. Source-frame disposition is separately disjoint: at closure, `cause_requested_frames = total_frames + cause_excluded_frames + cause_failed_frames`. Complete samples excluded by the evidence policy enter excluded; missing/corrupt payloads or failed mandatory artifact commits enter failed. Do not count the same sample in both, even if several diagnostic faults apply.

The examples below are **illustrative excerpts**, not captured evidence or complete session files. All displayed JSON is valid; ordinary camera/engine/object-node fields are omitted for space and retain their existing serialization. The corresponding three-frame event examples assume all three frames met the same declared measurement criteria. `frame_index`/`start_frame` are engine indices; `session_index` and frame lists are published indices.

**Natural MISS-2 — annotation.json excerpt:**

```json
{
  "label_schema": 2,
  "label_schema_minor": 1,
  "natural_metadata_version": 1,
  "natural_causes": [{
    "cause_event_id": "run7:cause3",
    "cause_id": "MISS-2",
    "cause_family": "content_error",
    "injection_mode": "natural",
    "mechanism": "engine_default_texture_assigned_to_colour_parameter",
    "aimed_target": "FixturePanel",
    "parameters_before": {"slot": 0, "parameter": {"name": "Albedo", "association": "global", "index": -1}, "texture": "/Game/Fixture/T_Colour"},
    "parameters_applied": {"texture": "/Engine/EngineResources/DefaultTexture"},
    "arm_request_id": 61,
    "revert_request_id": 64,
    "recovery_request_id": 66,
    "effect_event_ids": ["missing_texture@1001|FixturePanel"],
    "restore_status": "verified"
  }],
  "anomalies": [{
    "event_id": "missing_texture@1001|FixturePanel",
    "anomaly_type": "missing_texture",
    "injected_frames": {"start_frame": 20, "end_frame": 22, "frame_count": 3, "span_frame_count": 3, "frame_indices": [20,21,22]},
    "affected_frames": {"start_frame": 20, "end_frame": 22, "frame_count": 3, "span_frame_count": 3, "frame_indices": [20,21,22]},
    "transition_frames": [],
    "transition_reasons": {},
    "bbox_source": "drawn",
    "observable_frame_count": 3,
    "unmeasured_frame_count": 0,
    "observability_measured": true,
    "manifested": true,
    "mask": {"provided": true},
    "injection_mode": "natural",
    "cause_id": "MISS-2",
    "cause_family": "content_error",
    "cause_event_id": "run7:cause3",
    "effect_observation": {"state": "observed", "method": "rt_state_mask_clean_control", "first_request_id": 61, "last_request_id": 63, "observed_frames": 3, "component": "FixturePanel.Mesh", "slot": 0},
    "severity": {"metric": "mean_max_rgb_delta", "unit": "code_value_8bit", "min": 17.5, "max": 18.5, "mean": 18.0},
    "mask_measurement": {"method": "original_custom_depth"}
  }]
}
```

**The MISS-2 labels.jsonl entry at published frame 20** (pretty-printed here; one physical line in the file):

```json
{
  "frame_index": 1001, "session_index": 20,
  "image": "Actual_Frames/frame_00020.png", "width": 1280, "height": 720,
  "anomaly_present": true, "visible_positive": true,
  "mask_file": "target_mask/frame_00020.png", "mask_state": "present",
  "anomalies": [{
    "id": "missing_texture", "event_id": "missing_texture@1001|FixturePanel",
    "target_name": "FixturePanel", "start_frame": 1001, "labelled": true,
    "bbox_valid": true, "bbox_px": [100,100,64,64], "bbox_drawn_px": [100,100,64,64],
    "mask_value": 200, "target_pixels": 4096, "target_drawn_pixels": 4096, "observable": true,
    "injection_mode": "natural", "cause_id": "MISS-2", "cause_family": "content_error", "cause_event_id": "run7:cause3",
    "effect_observation": {
      "state": "observed", "method": "rt_state_mask_clean_control",
      "source_key": {"run_epoch": 7, "owner_generation": 2, "request_id": 61, "sample_ordinal": 23, "sample_game_frame": 1001, "family_id": 901, "view_index": 0},
      "baseline_request_id": 60, "component": "FixturePanel.Mesh", "slot": 0,
      "parameter": {"name": "Albedo", "association": "global", "index": -1},
      "material_generation": 3, "resource_generation": 4,
      "material_state_valid": true, "uniform_texture_matches": true, "resource_valid": true,
      "target_mean": 18.0, "noise_bound": 0.25, "visible_threshold": 4.0,
      "control_mean": 0.10, "control_noise_bound": 0.20, "unit": "code_value_8bit"
    },
    "severity": {"metric": "mean_max_rgb_delta", "unit": "code_value_8bit", "value": 18.0},
    "mask_measurement": {"method": "original_custom_depth"}
  }]
}
```

**Controlled MISS-1 — annotation.json excerpt:**

```json
{
  "label_schema": 2, "label_schema_minor": 1, "natural_metadata_version": 1,
  "natural_causes": [{
    "cause_event_id": "run7:cause4", "cause_id": "MISS-1", "cause_family": "code_defect", "injection_mode": "controlled",
    "mechanism": "explicit_engine_default_material_calibration",
    "aimed_target": "FixtureCube",
    "parameters_before": {"slot": 0, "raw_override": null, "override_array_length": 0},
    "parameters_applied": {"material": "/Engine/EngineMaterials/WorldGridMaterial"},
    "arm_request_id": 81, "revert_request_id": 84, "recovery_request_id": 86,
    "effect_event_ids": ["missing_texture@1021|FixtureCube"], "restore_status": "verified"
  }],
  "anomalies": [{
    "event_id": "missing_texture@1021|FixtureCube", "anomaly_type": "missing_texture",
    "injected_frames": {"start_frame": 40, "end_frame": 42, "frame_count": 3, "span_frame_count": 3, "frame_indices": [40,41,42]},
    "affected_frames": {"start_frame": 40, "end_frame": 42, "frame_count": 3, "span_frame_count": 3, "frame_indices": [40,41,42]},
    "transition_frames": [], "transition_reasons": {}, "bbox_source": "drawn",
    "observable_frame_count": 3, "unmeasured_frame_count": 0, "observability_measured": true, "manifested": true, "mask": {"provided": true},
    "injection_mode": "controlled", "cause_id": "MISS-1", "cause_family": "code_defect", "cause_event_id": "run7:cause4",
    "effect_observation": {"state": "observed", "method": "rt_state_mask_clean_control", "first_request_id": 81, "last_request_id": 83, "component": "FixtureCube.Mesh", "observed_frames": 3},
    "severity": {"metric": "mean_max_rgb_delta", "unit": "code_value_8bit", "min": 23.5, "max": 24.5, "mean": 24.0},
    "mask_measurement": {"method": "original_custom_depth"}
  }]
}
```

**The MISS-1 labels.jsonl entry at published frame 40:**

```json
{
  "frame_index": 1021, "session_index": 40,
  "image": "Actual_Frames/frame_00040.png", "width": 1280, "height": 720,
  "anomaly_present": true, "visible_positive": true,
  "mask_file": "target_mask/frame_00040.png", "mask_state": "present",
  "anomalies": [{
    "id": "missing_texture", "event_id": "missing_texture@1021|FixtureCube",
    "target_name": "FixtureCube", "start_frame": 1021, "labelled": true,
    "bbox_valid": true, "bbox_px": [200,100,64,64], "bbox_drawn_px": [200,100,64,64],
    "mask_value": 200, "target_pixels": 4096, "target_drawn_pixels": 4096, "observable": true,
    "injection_mode": "controlled", "cause_id": "MISS-1", "cause_family": "code_defect", "cause_event_id": "run7:cause4",
    "effect_observation": {
      "state": "observed", "method": "rt_state_mask_clean_control",
      "source_key": {"run_epoch": 7, "owner_generation": 2, "request_id": 81, "sample_ordinal": 43, "sample_game_frame": 1021, "family_id": 921, "view_index": 0},
      "baseline_request_id": 80, "component": "FixtureCube.Mesh", "slot": 0,
      "material_state_valid": true, "material_matches_engine_default": true, "uniform_resources_valid": true,
      "target_mean": 24.0, "noise_bound": 0.25, "visible_threshold": 4.0,
      "control_mean": 0.10, "control_noise_bound": 0.20, "unit": "code_value_8bit"
    },
    "severity": {"metric": "mean_max_rgb_delta", "unit": "code_value_8bit", "value": 24.0},
    "mask_measurement": {"method": "original_custom_depth"}
  }]
}
```

MISS-1's family is its catalog grouping, **not evidence of an actual code/shader failure**. The explicit mechanism string and controlled mode are mandatory in its documentation; the actual configured default path is recorded rather than assuming WorldGridMaterial on every host.

### B6. Per-type choice: console and dashboard

Proposed console interface (new commands, not claims that these exist today):

```text
IAI.Injection.Mode missing_texture controlled
IAI.MissingTexture.ControlledRecipe plugin_checker
IAI.MissingTexture.ControlledRecipe engine_default
IAI.Injection.Mode missing_texture natural
IAI.Injection.Mode missing_texture
```

The last command reads back effective choice/capability. Controlled keeps the shipped checker by default; `engine_default` selects MISS-1. Natural selects MISS-2. Other types report controlled-only until their own natural implementation is qualified. Attempting an unsupported natural type gives a clear refusal, never a controlled fallback disguised as natural. The choice is per type, independent of pool on/off and of the dashboard's existing auto/targeted capture mode. Freeze choices for a running capture/attempt; console or dashboard edits take effect at the next safe run boundary and read back that status. Existing UV/normal submodes and blur hold/proxy routing keep their meanings.

Owner UI choice:

| Shape | Owner-facing behavior |
|---|---|
| **A — recommended: mode beside each pool row** | Existing on/off switch plus `Controlled / Natural` choice. Missing texture's controlled choice reveals `Plugin checker / Engine default`. Targeted capture shows the same choice under its anomaly selector. Unsupported natural choices are visibly unavailable with a short explanation. No new anomaly cards or IDs. |
| **B — compact: row details** | Keep the list unchanged; selecting a row opens one details panel with mode and controlled variant. Same per-type configuration and readback; less visual clutter, one extra interaction. |

Do not choose natural as the default until the owner decides after proof. Show “Checking effect”, “Effect observed” and “Restoring” during a pilot; successful-event totals must not advance merely because the material setter ran. Technical probe counters belong in the diagnostic reader, not the ordinary capture flow.

| File(s) | Planned UI/protocol work |
|---|---|
| `I/Public/AnomalyCatalogTypes.h`, injector/auto subsystem files | Advertise per-type modes and controlled recipes, store/read back the selected recipe, validate commands and freeze them into the attempt. No new catalog anomaly IDs. |
| `P/Source/AnomalyControlServer/Private/ControlSnapshot.{h,cpp}` | Capability/configuration maps alongside existing `auto.pool`; pilot status and attempted/observed counts. Old clients still receive unchanged pool booleans and IDs. |
| `P/Source/AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp`, `ControlProtocol.{h,cpp}` | Validate additive `auto_config.injectionModes` and recipe selections, authenticate through existing rules, reject unsupported values and acknowledge safe-boundary changes. Omitted keys preserve existing configuration. |
| Dashboard `src/types.ts`, `src/transport/AnomalyClient.ts`, `src/transport/validate.ts` | Optional capability/mode/status types and messages, old-server fallback, strict value validation and server-authoritative readback. `src/transport/protocol.ts` needs no binary-preview framing change. |
| Dashboard `src/components/SetupPanel.tsx`, `src/anomalies.ts`, `src/store.ts`, `src/appStore.ts`, `src/views/CaptureView.tsx`, `src/styles.css` | Implement owner-selected A/B in the actual pool/targeted panel; variant-specific explanation; queued/confirmed configuration; truthful active strip. Preserve capture/export UX. |
| Dashboard `src/mock/mockGame.ts`, `src/transport/AnomalyClient.test.ts`, `src/store.test.ts`; **NEW** `src/components/SetupPanel.test.tsx` if supported by the existing harness | Protocol/capability old/new cases, invalid selection, stale optimistic value, mode frozen through a running attempt, unsupported type and checker/default/natural distinction. Avoid a new test-framework dependency merely to render a switch; use existing tests or browser QA if needed. |

Dashboard paths refer to `D:/IntrusiveAnomalies/anomaly-dashboard`, inspected read-only at `master 7ea2e51`, clean status. Recheck that independent repository at implementation; do not assume the old `WebClient/spike-client.html` is the product dashboard. No dashboard redesign, packaging dependency upgrade or unrelated repository merge is implied.

### B7. Optional cause metadata for existing recipes

Freeze the actual effective subtype/mode at application, not at label-write time. Add metadata through `I/Public/AnomalyTelemetry.h`, `IAnomaly`/active-record plumbing and the label writer; source it in these exact anomaly files:

| Existing source | Metadata when applicable | What stays controlled |
|---|---|---|
| `A/Anomaly_LodCorruption.{h,cpp}` | `LOD-1`, `content_error`, “forced LOD left on” | The existing forced-LOD event and its frame/mask policy |
| `A/Anomaly_TexCorrupt.{h,cpp}` | `NRM-1`, `content_error`, **only** normal `green_flip` | UV, normal invert, proxy blur and all copy routes otherwise receive no cause ID |
| `A/Anomaly_CameraClipping.{h,cpp}` | `CAM-1`, `content_error`, “near plane set too large” | Existing view/global scope; do not call it camera collision failure |
| `A/Anomaly_Blinking.{h,cpp}`, `I/Private/AnomalyHiddenClass.cpp` | `GONE-3`, `code_defect`, **only captured mode 0** | Mode 1 receives no cause ID; ordinary blinking remains controlled in either mode |

No other shipped recipe receives a cause ID. In particular, do not label the release blur hold BLUR-1 solely because it uses a real streaming lever. Do not add `time_dilation` or `lighting_mismatch` to the qualified menu. Metadata does not fix mode-0 hidden masks or its unconditional-show restore behavior; this task does not claim those legacy defects solved. The owner informs the client of the additive keys and the pilot behavior **before the first natural build ships**.

## C. MISS-1 — controlled R1 calibration

**Mechanism:** obtain `UMaterial::GetDefaultMaterial(MD_Surface)` and explicitly assign it to the selected component's material slots. The configured default is authoritative, not a hardcoded stock path. Its presence is required by engine initialization (`E/Private/Materials/Material.cpp:503–569`); usable shaders for the particular geometry and actual staged behavior still need proof. This does not induce a compile failure, remove usage flags, set a null texture or pretend to exercise a fallback trigger. Code verification row 3 is upheld: usage checks during PIE tick/draw take the in-game branch; checks outside PIE can repair/dirty assets. This pilot performs neither experiment.

**Files:**

| File(s) | Proposed MISS-1 work |
|---|---|
| `A/Anomaly_MissingTexture.h`, `.cpp` | Dispatch the existing controlled checker unchanged, or the new controlled calibration adapter; same `GetId() == missing_texture`. Do not reuse the checker's “derived from checker” owner-wide sweep for the engine default, which may legitimately be present elsewhere. |
| **NEW** `A/Anomaly_MissingMaterialCalibration.h`, `.cpp` | Default-material resolver, component/slot plan, explicit apply/RT-watch descriptors and RM restore. Uses the foundation even though the resulting mode is controlled. |
| **NEW** `A/AnomalyMaterialTransaction.h`, `.cpp` | Shared RM machinery for both pilots: raw overrides, array lengths, effective materials, strong retention, exact component/mesh identity, owned-slot comparison, queued retirement and restore verification. No name-based successor substitution. |
| Injector catalog/console files in §B | Separate `plugin_checker`/`engine_default` controlled recipes without registering a second missing-texture anomaly. Include the configured default in preparation/readiness, not the release's plugin-checker-only prewarm list. |
| `C/Private/AnomalyMechanismProbe.cpp`, attempt measurement/publisher | Default material/uniform state, measured nonzero original mask and clean-reference change, all keyed to the captured request. Default binding alone never passes. |

**Eligibility:** a clean, visible, currently registered, maskable component with retained material state and adequate observed support. First proved scope is ordinary non-Nanite static-mesh components with opaque stable geometry; masked components may join only with demonstrated stable coverage and accurate baseline/change measurements. This is a declared measurement scope, not a claim that every other component class is impossible. No suffix/name blacklist. A multi-component actor changes only the chosen component; install its positive scope. For a multi-slot component, swap all material sections covered by that component's mask as one material-assignment cause, or do not claim a section-specific mask. Verify the resulting footprint; removal of WPO, opacity or material-driven geometry can invalidate the clean comparison and must revert/quarantine, not count as a texture appearance proof. A target already showing the same configured default is an expected no-effect, not a compatibility failure.

**Restore RM, shared with MISS-2:**

1. Before touching any slot, retain exact component identity/generation, mesh, slot count, **raw** override array including nulls and original array length, effective original material, all plugin-changed flags/tags, and the intended installed pointer. Retain original materials/host MIDs strongly. Do not call `Modify`, `PostEditChange`, `MarkPackageDirty` or save a host asset.
2. Restore only slots still carrying this transaction's installed pointer on the original component/mesh identity. `SetMaterial(slot,savedRaw)` includes null for inherited slots. Do **not** put the resolved asset material into a formerly null override. `MeshComponent.cpp:48–94` shows the setter grows the array and updates render/streaming/physical-material state; equal effective appearance does not prove exact override restoration.
3. Restore original array length only where the tail is provably this transaction's added empty storage and the host has not extended/replaced it; never trim a foreign entry. Explicitly verify inherited and explicit-null slots, zero-length arrays and multi-slot tails. Notify normal render state through the setter/dirty mechanism as needed after an owned trim.
4. A host slot/mesh/component takeover is lost ownership: leave the new host state intact, stop the attempt and report restore not verified. Do not resolve a newly created same-named component and write old state into it. Reverting the original material pointer automatically preserves host-MID changes that happened on that original object.
5. Verify raw state equality plus effective original material/uniform state on RT, stencil/flag restoration and clean picture recovery. Release private objects only after pending requests and a bounded render-retirement fence finish. A cleaned-up pointer, a null sweep or a restored scalar alone is not an exact-restore pass. No global flush per frame.

**Gates:** changed pictures with engine default in PIE and staged, ≥3 successful targeted and ≥3 automatic calibration events per certifying configuration; correct default resource and zero-frame edges; same-default no-effect control; no-op apply lever; C1 changed/C2 unchanged mask control; raw-null/array-length exact restore; mid-event host takeover; stop/world teardown; actual configured-default staging. No shader-failure claim is tested or certified. MISS-1 runs alongside the foundation before natural MISS-2, as R1 requires.

## D. MISS-2 — natural placeholder assignment on the m53 MID route

**Mechanism:** keep the host's compiled material graph and bind `GEngine->DefaultTexture` to one selected **full texture-parameter identity** on a plugin-owned transient MID. This is a wrong placeholder/content assignment, not evidence that the host's loader failed. The engine default texture is normally `/Engine/EngineResources/DefaultTexture` (`BaseEngine.ini:149`; Code verification rows 9–12). Its exact pixels were not established by the earlier source study; call it the engine default texture until the future staged/PIE inspection proves its appearance. Never substitute `GWhiteTexture`, a null setter, or a fabricated checker and still call that MISS-2.

### D1. File-level implementation

| File(s) | Proposed change |
|---|---|
| **NEW** `A/Anomaly_MissingTextureNatural.h`, `.cpp` | Prepare/arm/restore adapter using the common attempt coordinator and RM. Select a parameter candidate, create an identity MID, then set the default texture after its identity control. Same anomaly ID as existing missing texture. |
| **NEW** `A/TexCorruptMaterialBindings.h`, `.cpp`; `A/TexCorruptTree.cpp`, `A/TexCorruptCore.h` | Factor/reuse m53's active compiled-uniform enumeration and draw-readiness utility (`TexCorruptTree.cpp:1033–1085`), with full association/index/type and resource generations. Keep legacy copy-family admission in its own policy. MISS-2 does not need redraw, a reduced-mip copy, full residency, eightfold texture ratio, copy format eligibility or texture nonuniformity. |
| **NEW** `A/AnomalyHostMaterialClone.h`, `.cpp` | Sibling-MID construction for source MIDs, effective-parameter snapshot/verification and host-change detection. A plain UMaterial/MIC uses an ordinary child MID. All clone instances remain private to the selected component/slot transaction. |
| `A/AnomalyMaterialTransaction.{h,cpp}` | Raw slot/array RM restoration, resources retained through capture and GPU work, host takeover and abort cleanup. |
| `A/Anomaly_MissingTexture.{h,cpp}`, subsystem/catalog/telemetry | Route natural mode to this adapter; keep plugin checker and MISS-1 as controlled choices. Optional provenance and cause counts use §B5. |
| `C/Private/AnomalyMechanismProbe.{h,cpp}` | RT effective material and every occurrence of the selected parameter; default texture/resource validity and non-target binding parity; unknown on generation or permutation change. |
| Bench fixture generator, checker and docs in §E | Effective/unused parameter twins, same-default no-effect, packed/reused inputs, host-MID identity and mutation, VT and partial-section boundaries, staged asset/resource availability. |

Reusing the m53 **binding route** does not mean calling its full UV/normal copy admission tree. For example, a uniform red colour texture can visibly change when replaced by DefaultTexture; `texture_uniform` is not a valid blanket MISS-2 veto. A shared original texture is not a refusal: no shared host texture or original MID is mutated. F5's private-copy snapshot problem is therefore not intrinsic to this lever, although the new observer still must handle resource generations correctly.

### D2. Selecting the colour parameter and measuring its contribution

1. Enumerate the **effective shader map for the current feature/quality level**, through m53's host-draw readiness path. Pick named ordinary 2D uniform parameters; constants with no settable parameter identity are reported separately. Reject unexpected default-material fallback, missing required draw shaders, incompatible resources and unmeasurable scope with their actual reasons, not a global editor shader-pending count.
2. Initial validated colour class: sRGB source, not normal-map compression/normal class, ordinary Texture2D, live compatible resource, full `FMaterialParameterInfo` (`Name`, `Association`, `Index`) present in compiled uniforms. Names such as Albedo/BaseColor may **rank** candidates, never establish eligibility by themselves. Linear/data bindings outside this first validated colour scope are reported as unassessed colour candidates, not declared universally incapable of a missing-texture effect.
3. Cross-check effective GT texture against the compiled entry; after arm, confirm the RT entry resolves the expected replacement. A parameter present in a material's editable list but optimized out of compiled uniforms is not effective. A parameter still present in uniforms can nevertheless be multiplied by zero, feed an unused visible channel or be visually cancelled: **only the attempt picture test decides visibility**.
4. On a wrong visual guess, restore and count no-effect/ambiguous/unknown as appropriate. Try another qualified candidate at a subsequent cadence after recovery; do not retitle an invisible swap a success or lower the threshold until it passes. Known same-default parameters can be detected early; the bench also deliberately exercises the armed no-effect path to prove it cannot count.
5. Retain inherited addressing/sampler mode, scalars/vectors, static permutations and every untouched binding. This is a binding substitution, not a pixel copy. The replacement's own dimensions/content are the intended content mistake; do not force the original resident, alter texture quality or repair host assets to make it eligible.

**Footprint limit:** F2 supplies a component mask, not a material-section mask. The first MISS-2 route selects a component whose rendered sections all use the selected material/parameter assignment (a one-material component is the main positive). If only one of several differently shaded sections can be changed, report `section_footprint_unmeasured` and the lost-yield count; do not label the whole component as a precisely measured changed part. Admit a multi-component actor's qualifying component with C2 excluded. Later section-level measurement is a separate implementation, not an undocumented requirement to corrupt every actor part.

### D3. Packed/reused parameters and host MIDs

**Packed/reused:** preserve the full parameter key and collect all compiled occurrences. One parameter may affect colour plus roughness, emissive, opacity or another graph input; the plan does not promise other outputs stay unchanged when they reuse it. Repeated samples with different addressing/UVs are still the same content assignment. Record occurrences/sampler sources inside the observation. Distinct bindings retain their effective values; a “base colour” name does not authorize changing a different layer with the same name. A coupled opaque colour/roughness positive is supported if the identity and picture/mask tests pass. Coverage/WPO changes that defeat the clean footprint or move effects outside measurable object support are unknown/refused measurement in this v1 and reverted. Do not borrow GONE's unimplemented companion or invent LIGHT-5's deferred `effect_mask`.

Private MID scope should prevent direct material-binding collateral, but not every possible lighting/reflection side effect. Monitor the control region and the visible binding-user inventory through the event. If a second visible component truly uses the altered binding, it requires its own measured event and recovery window; it is not covered by the aimed object's box. Unaccounted outside-object change makes the affected source samples unknown/excluded and the attempt stop. It is not called clean, nor claimed as full receiver/shadow-footprint support. Later BLUR-1 needs the full shared-resource user graph, not a relaxed version of this check.

**Host MIDs:** calling `UMaterialInstanceDynamic::Create(hostMID,...)` is invalid for this purpose. UE deliberately removes MID parents and falls back to the surface default (`E/Private/Materials/MaterialInstance.cpp:764–791`). The current m53 route avoids that case; blindly removing its `host_mid` refusal would change the graph.

Instead create a sibling rooted at the host MID's valid non-MID parent, preserving that parent's compiled static permutation. `CopyMaterialUniformParameters` is a useful public starting point (`MaterialInstanceDynamic.h:140`, `MaterialInstance.cpp:4497–4598`), but **not an unverified universal clone**: its base-material enumeration assigns some entries by name and explicitly notes fonts. Verify/copy effective active numeric/texture values by full identity and current feature/quality, including inherited values. Preserve applicable runtime/base-property/physical-material behavior; if a relevant state cannot be reproduced, refuse for identity mismatch. Do not alter static switches or recompile the host.

Install the unmodified sibling first, using RM and provisional samples, and require unchanged image, uniforms and footprint. Only then bind DefaultTexture. This supports stable host MIDs without touching the original. Keep the original MID alive and check its effective-parameter digest/slot identity before each capture. If gameplay changes that original during the attempt, the initial implementation cancels, restores the original pointer with its **latest host values**, and marks uncertainty until recovery; it does not freeze ongoing host animation and call it the natural cause. Continuously mirroring arbitrary host MID animation is outside the first pilot. Name/association collisions, parent changes, shared host MIDs, scalar/vector overrides and unrelated texture overrides need explicit fixture cases.

**VT:** skip any selected compiled VT/RVT binding or parameter also used in a compiled VT stack. Never put ordinary DefaultTexture into it: UE skips non-VT layers and warns (`E/Private/Materials/MaterialShared.cpp:3711–3725`). A material may still be eligible through a distinct ordinary colour parameter while its unrelated VT bindings remain untouched and pass the clone/identity control. Report slot/binding denominators: VT-heavy content can yield few or zero ordinary colour candidates. No VT branch, private VT asset or module enters this work.

**RM and gates:** use §C's exact transaction restore, not parameter-by-parameter writes to the host MID. A missing/default asset or bad sampler/resource refuses before count; GT getter equality is only a wiring check. Prove MIC and sibling-MID positives, unused/zero-strength/no-effect twins, packed reuse, layer identity, untouched users/parameters, stale resources, exact restoration and host takeover in **PIE and staged**. Verify DefaultTexture and the configured default material actually load/render in the candidate package. These engine defaults normally arrive through engine initialization; no claim that a stock path is always cooked under arbitrary host configuration is needed. If a stock fallback asset is newly required, add an explicit plugin-owned cook reference and repeat the staged gate; do not silently choose another texture. `FlatNormal` belongs to the later NRM-3 job, not this cook scope.

## E. Predictions, fixtures and gates

### E1. Evidence plan before any future leg

Create **NEW** `P/docs/predictions/093-natural-foundation-miss.md` in J1 **before its first reproduction or candidate leg**, with dated, append-only amendments before later experiments. This report is a plan, not that future binary-specific predictions file. Freeze positive fixture names/classes/material cases, expected eligibility and visible yield, threshold/AA units, timing/memory budgets, baseline/candidate commit and binary hashes, cooked containers/assets, tick order, capture settings, exact source-sample membership expectations and the can-fail device for every gate. Never tune away a failed positive by removing it from the declared set after its leg.

Use the house bar in release `docs/PRE-DELIVERY-CHECKLIST.md` §0: **running PIE AND staged Development/Test before any office candidate**, targeted AND automatic behavior, both readiness directions, census performance, ordinary and office-like content, visible picture proof, zero-frame label edges, and a gate that can actually fail. A certifying positive leg needs **at least three counted visible events**; zero events is a STOP on a declared positive. Null/refusal/uncertain negative legs deliberately require zero successes and are paired with certifying positives, not called invalid for meeting their prediction. Exercise both natural and synthetic tick orders where the capture gate supports them; a missing mode must be recorded, not silently removed from the matrix.

Fixture proposal:

- **StackOBot bench:** extend the existing texture-corruption fixture with a large ordinary colour-textured panel, configured-default-material twin, DefaultTexture-bound twin, optimized-out and multiplied-away parameter twins, a uniform-colour positive, inherited/null/explicit material slots, a two-component C1/C2 actor, mixed-section components, masked/WPO coverage controls, layered same-name parameters, stable and changing host MIDs, shared original texture/MID users, ordinary-plus-VT materials, and a true VT-only negative. Add a game-only marker for home owner-view proof. No production host assets are changed.
- **Office-like fixture:** retain/recreate the 091-05 co-user/part-scope cases plus ordinary parameterized surfaces among a VT-heavy/multi-component population, runtime MIDs, reduced residency and the 090-10c bias/quality/pool stressors **together**. Use a second independent scene; the retained Lyra worktree can provide the house second-host fixture if available. Record the actual fixture/engine compatibility, not a promise that its material names mimic an office host. A 15,000-actor census leg retains the <100 ms frame bound and progress/denominator checks. Predeclared ordinary positives remain in the stress scene so zero eligibility cannot pass unnoticed.
- AA-off is the exact pixel-edge arbiter; FXAA and the normal TAA/TSR capture setting get separate legs. Under temporal AA the measured visible frames, onset/offset exclusions and recovery have to agree with the image; merely restoring the default transition constants is not a proof. Host quality is never changed to make a failed control pass. Deliberate fixture stress settings are predeclared and restored after each leg.

### E2. Gate matrix — all controls run locally, none in this planning job

New fault levers below are proposed under **one** `IAI.Bench.NaturalGate <case>` dispatcher using the existing `AnomalyBenchGate`, absent/masked without the bench gate and compiled out of Shipping. The names describe distinct cases; actual numeric IDs are frozen in predictions. Existing levers/old binaries remain preferable where they expose the real defect. A can-fail may make the certifying checker fail, or prove a negative gate blocks and counts the fault; predictions distinguish these outcomes. Do not falsify only the telemetry field the checker then reads.

| Gate | Required positive/negative evidence | Can-fail device |
|---|---|---|
| **G-BASE** | 091-05 F2 scope, F3 alpha and F6 identity; m52/proxy/UV/normal regression; 0/0/0 defaults and explicit overrides; source/candidate manifest | Old `d660b5d` scope/alpha controls; `NoPartScope`; wrong event ID on a transition-free row must fail CLI and hook |
| **G-OWNER** | New-window and selected-viewport PIE, extra realtime editor world, per-client PIE where supported, standalone/staged; zero wrong pictures and usable frame counts; late viewport/resize/start restart recovery | Archived `3ADC2EA4` delayed-viewport control must reproduce zero written vs requests; old F1 bypass/frame-alias path must fail game-marker check; candidate `OwnerBypass` plus second family |
| **G-KEY** | Colour, mask and mechanism join by request/family/view/generation, correctly handle missing owner render and out-of-order completion | `ProbeRequestSkew`, duplicate/family-key collision, epoch reset, delayed old result; checker rejects a deliberately accepted skew, protected path excludes/counts it |
| **G-CLEAN** | No cause arms from expired/dirty/other-owner census; clean identity MID baseline; current ordinary positives still yield | `DirtyBaseline`/`ExpiredCensus` and a moved camera or component between baseline and arm; neither can be relabelled a no-effect |
| **G-MECHANISM** | MISS-1 default material; MISS-2 effective ordinary parameter; required shader/resource state valid on the same sample | `SkipSetter`, changed parameter association/index, forced probe-resource mismatch. A GT success with unchanged picture must not pass |
| **G-PICTURE** | Declared positives clear visible/clean-null thresholds in measured footprints; independent source-PNG checker and visual inspection agree | Real shader fixture with effective uniform multiplied by zero; `IdentityReplacement`; a controlled weak change between noise and visible thresholds must enter ambiguous and not count |
| **G-NO-EFFECT** | Already-default-material MISS-1 and already-DefaultTexture MISS-2: attempts recorded, successes/events/root causes zero, owned restore verified | Deliberately force the “count on Apply” mutant; the no-effect checker must fail it. Production early same-value detection and bench forced-arm no-op both exercised |
| **G-MASK / m26** | Every labelled pilot frame has its nonzero measured same-key mask, saved PNG and matching counts/bounds; no projected/old-mask substitute | Drop mask readback, zero tag, suppress custom-depth pass and mask-write fault. Natural must refuse/exclude even when old m26 would admit NOT_MEASURED; an early-label mutant must fail |
| **G-EDGES / PUBLISH** | Label start/end and measured mask membership **0 source frames off**; published image/mask/annotation/JSONL/video indexing agrees after exclusions; no missing-key frame published clean | Onset/offset skew ±1; `CommitBeforeEvidence`; delayed result after stop; colour-write/mask-write failures and failed commit. Independent edge checker catches skew, protected publisher leaves no positive bundle |
| **G-SCOPE / COLLATERAL** | C1-only changes have zero C2 mask pixels and C2 RGB within clean noise; reused binding effects mapped separately; truthful missed-aim case | Intentionally install the private binding on a second fixture component; no-effect aimed twin plus visible second component must yield collateral event(s), aimed-success=0, missed-aim=1. Dropping its event must fail |
| **G-RM** | Raw slot pointer/null/array length and mesh/material/parameter state restored; picture recovered; no host dirtiness, residual MID/tag or leaked retained resource | `RestoreResolvedInsteadOfRaw`, leave one override/tail/tag, fail after first slot; checker must detect each. State readback alone cannot pass picture recovery |
| **G-HOST** | Stable sibling-MID identity exact within noise, non-target values preserved; host takeover/mutation never overwritten; same-name successor untouched | Wrong MID parent, wrong layer identity, ignored host scalar write, replaced component. Protected path cancels/restores or reports lost ownership; clone-before/after pictures detect a false identity claim |
| **G-ELIGIBILITY** | Nonuniform and uniform colour positives; missing required shaders/default-path/VT-only negatives; ordinary slot on mixed VT material separately assessed; no host quality repairs | Frozen fake eligibility output admits an incompatible fixture or erases its VT refusal; checker fails. Do not deliberately bind an invalid VT resource to provoke a crash |
| **G-LIFECYCLE / BOUNDS** | One cause across hold/revert/recovery; stop, GC, target loss and teardown close counts; 256 MiB/16-sample bounds and census <100 ms; no deadlock/leak | Concurrent apply during recovery, permanent missing receipt, tiny evidence budget, delayed writer, teardown with pending RT; no next cause on an unresolved tail |
| **G-SCHEMA / COUNTS / UI** | All old key meanings/IDs intact; R8 whitelist, matching events, partition equations; optional provenance correct when supplied or omitted safely; metadata-off membership identical; console/dashboard echo same per-type state | Wrong mode/MISS-1 called natural, duplicate success, orphan event ID, metadata toggle changing a frame list, old-server capability absence, rejected config shown optimistically as applied. A bad optional cause link must be detectable and safely omittable without changing the event |
| **G-PACKAGE / REGRESSION** | Candidate editor and staged builds/strict checks clean, descriptor/lever audit, expected engine defaults present, no bench material/module in delivery staging; every changed supported anomaly visibly works | Existing lever-audit and descriptor selftests; staged manifest wrong asset/binary; separate known-bad fixture path. Staged loading/rendering is required, a filename/string hit alone is not success |

For the scope-control image gate, reuse the 091-05 clean envelope +0.05 and zero pixels over 12 levels where applicable, with its 20-level failing control; do not replace it with unattainable byte equality under temporal noise. RM still requires exact **owned state** equality. Include low-signal, temporal, exposure/control disturbance and recovery tails in the prediction table, not only obvious checker positives.

**m26 specifically stays on.** It currently removes only manifested MEASURED_ZERO events and admits NOT_MEASURED (`AnomalyCaptureSubsystem.cpp:8941–9025`). Neither that admission nor a pre-arm census mask meets the pilot gate. Only committed effect-frame measurements contribute to the promoted event's m26 record: do not let a clean baseline's nonzero mask hide a later zero. A zero/missing pilot mask causes pre-publication exclusion and restore, so deleting an event cannot leave unexplained changed images labelled clean. If final m26 nevertheless removes a supposedly qualified pilot event, final reconciliation fails and the run is not released; do not silently omit its labels. No reference companion is used in MISS-1/2.

### E3. Proposed fixture/checker/doc files

- `P/Source/AnomalyBench/Private/AnomalyBenchModule.cpp`; **NEW** `AnomalyNaturalFixture.h`, `.cpp`: fixture commands, late-viewport and actual material/parameter fault devices, all under the single bench gate. Extend the existing two-part actor only if needed; do not place fixture helpers in a host-game module.
- **NEW** `P/tools/create_natural_fixtures.py`: reproducible benchmark-only assets/scenes and explicit cook inputs; no office asset edits. Reuse `tools/create_texcorrupt_assets.py` support without adding bench content to the client plugin.
- **NEW** `P/tools/natural_attempt_check.py`, `tools/test_natural_attempt_check.py`: independently read saved PNGs/masks, annotations/JSONL and request evidence; check noise/visible classification, index mapping, per-object effects, counts, exact restore records and diagnostic exclusions. Plant wrong pixels/one-frame edges/IDs/missing masks in copies to prove the checker fails. No prediction is derived solely from the label being tested.
- Bench evidence retains a **bounded diagnostic source-image/mask window**, including samples later excluded from the client sequence, so the independent checker can assess the original edges and ambiguity cases. It is explicitly outside `Actual_Frames`/client exports, with request/sample IDs and a declared byte/frame cap. Ordinary operation need not retain those diagnostic images; office inspection, if enabled locally, never transfers them home. This does not relax the in-memory publication cap.
- **NEW** `P/tools/natural_attempt_selftest.cpp`, `natural_probe_selftest.cpp`, `material_transaction_selftest.cpp`: meaningful state-machine, ownership/generation, outcome partition, full parameter identity and raw-array restore cases. Extend existing `capture_ownership_selftest.cpp` from 091-05, `mask_serve_selftest.cpp`, `stencil_scope_selftest.cpp`, `test_annotation_transitions.py`, `verify_capture.py` and change-oracle tests where they consume the new policy. Unit tests do not replace in-engine proof.
- **NEW** `P/tools/natural_office_readings.py`; extend `tools/OFFICE-CHECK.md`: local-only numeric extraction plus owner visual/edge readings, with missing fields reported as **unavailable**, never zero. It does not transfer files or contact a service.
- `P/docs/client-readme.md`, `docs/architecture.md`, `docs/PRE-DELIVERY-CHECKLIST.md`, `docs/gotchas.md`, `CLAUDE.md`, and a **NEW** dated `docs/sessions/` journal per job: accepted scope, settings/units, controlled/natural examples, attempt vs success, exclusion behavior, clone/VT limitations, evidence hashes and unrun gates. Preserve release history and append gotchas rather than rewriting them as if the release had these changes.

Build/cook/stage commands are deliberately not executed or supplied as an already-approved next action here. Future shader/content changes require the house cook/rebaseline record; same-source rebuilds do not justify swapping a previously pinned binary without new evidence. Code reviews the implementer's source/evidence; chat receives any failed premise or scope change before the next job is enlarged.

## F. Implementer jobs and office checkpoints

### F1. Serial job split

One Codex implementer, one bench, one brief at a time. Estimates include implementation and **one bounded 3–6 hour bench session** as applicable; they are engineering estimates, not an office delivery date. Persist exact source/binary/prediction/evidence at each boundary. If a job cannot finish its declared matrix in that session, report the unfinished gate and split a continuation; do not combine unreviewed extra legs or weaken the bar.

| Job | Scope and dependency | One-session proof / handoff | Estimate |
|---|---|---|---|
| **J1 — integration base** | New branch from d7eee97; six 091-05 commits, release defaults/docs retained; no natural mechanism yet | Predictions first; F2/F3/F6, mask-drop accounting and short controlled regression in PIE/staged. Candidate stays home until J2 | 3–5 h |
| **J2 — office-robust F1** | Port/complete saved WIP, immutable owner generation, ambiguity policy, numeric diagnostics | Old delayed-viewport can-fail; new-window/selected/client/extra-editor modes supported by bench, stale/resize control, staged pairing. **Office checkpoint I** only after home proof | 4–6 h |
| **J3 — attempt/census/lease** | Provider/types, scheduler provisional lifecycle, clean census, RM skeleton and cancellation | Bench-only identity/no-op cause exercises clean/dirty/stale admission, one cause through recovery, no premature public event and no stuck lease | 4–6 h |
| **J4 — mechanism + picture evidence** | MISS probe readers and m55 attempt-stage controls/thresholds; bench twins only | Real default/uniform observations vs no-op, wrong request/resource and ambiguous signal; both fixtures/PIE/staged. No production pilot toggle yet | 4–6 h |
| **J5 — publication barrier** | Pre-label evidence join, required mask-file ack, source/published index mapping and atomic bundle accounting | Forced lost/delayed/write-failed frames and ±1 edge controls; no clean contamination or labelled frame without mask; stop/closure/budget proof | 4–6 h |
| **J6 — additive labels and existing metadata** | R8 serializers/counts plus the four allowed controlled tags; finalize fixture schema and independent checker | JSONL/annotation/mask agreement, count equations, metadata-off identical membership, wrong-ID and mode/subtype can-fails; sample docs validated | 3–5 h |
| **J7 — MISS-1 calibration** | Production controlled engine-default adapter with complete RM | Targeted + automatic ≥3 visible events, already-default no-effect, C1/C2, staged/default readiness and restore matrix; foundation now qualified with calibration | 3–5 h |
| **J8 — MISS-2 ordinary material/MIC** | Effective ordinary colour parameter, private MID on m53 binding route, VT skip | Real/unused/zero-strength/same-default pairs; targeted + auto, ordinary+office-like fixture, no new copy restrictions; same-key edges/masks | 4–6 h |
| **J9 — host MID / reuse / churn** | Sibling clone and identity, layered/reused parameters, host changes, footprint boundaries | Stable MID and coupled opaque positives, wrong-parent/identity negatives, host takeover/GC/restore, collateral missed-aim and resource-generation tests | 4–6 h |
| **J10 — owner-selected UI and protocol** | UI A or B, per-type capabilities/recipes, authoritative status and numeric reader | Console/dashboard round-trip on running PIE/staged; old-server/unsupported/stale config tests; no data-label ID drift | 3–5 h |
| **J11 — candidate qualification** | Pinned final candidate; reconcile every changed file/build; final client wording and delivery staging checks | Full supported-anomaly regression, both pilots on both fixtures in PIE/staged, both tick orders/AA edge legs, ≥3-event positives, gate can-fails, leak/restore/throughput and packaging proof. **Office checkpoint N** | 4–6 h |

Total **40–62 h**, plus review/rework, cook availability and owner office time. J4/J5 are intentionally separate because a passing sidecar does not make the publication path safe. J8 is not an office-ready subset release before J9. No BLUR-1/GONE-1/NRM-3 or VT job is hidden in this list. Chat reviews this plan now; Code reviews each implementation report; **Chat review REQUIRED** for design departures and the final source/evidence disposition before merge/release. The owner's initial go/no-go covers only the approved job plan, not an unreviewed expansion.

### F2. Office proof: numbers only, typed by the owner

No office captures, logs, screenshots, assets or names come home. The numeric reader runs on the office PC; the owner transcribes its fixed-column line and the manual visual/edge counts. Codes: **host 1 = Concorde, 2 = Bates**. Candidate/build identity is a small agreed numeric receipt code whose home manifest fixes commit and hashes; do not ask the owner to transcribe project paths. Use `-1` for an unavailable reading and fail qualification; never fill an absent counter with zero.

**Checkpoint I after J2: prove (a) captures the intended view on the actual office setup.** Per host, run these bounded cases, with default async/SVE capture and the owner's normal capture resolution. Complete warmup before the requested-count interval.

| Case code | Owner action after the home candidate passes | Expected reading |
|---|---|---|
| **11** | Usual office PIE arrangement; three short UV events on a known working target | 360 requested samples; 360 written, ≥3 visible events; edge/mask/pairing errors 0 |
| **12** | Same arrangement; three shared-proxy blur events where the existing office case is eligible | Same. If no target is eligible, report that denominator and do not claim this blur case qualified |
| **13** | PIE in a new window with another realtime editor viewport open on a visibly different view; three UV events | Same; non-owner rejections >0; wrong-view pictures 0. This case distinguishes F1 protection from merely writing frames |
| **14** | Stop/restart capture after resizing/reopening the PIE window; same known UV target | Same stable interval after rebind; owner-generation/update reading changes when the viewport actually changes; no stale result accepted |

The exact **I form**, in this column order (all integers):

```text
host receipt case world_type net_mode owner_scene_known_start owner_target_known_start owner_updates context_world_null target_fallback_accepts rejected_nonowner accepted_ambiguous requested written visible_events mask_required mask_paired unpaired_dropped wrong_view_checked wrong_view_count missing_masks edge_events_checked max_abs_onset_error max_abs_offset_error label_identity_errors
```

Qualification: `owner_scene_known_start=1`; normally `owner_target_known_start=1`. If the latter is 0, the numeric diagnosis must show a later owner update and zero accepted ambiguous owners; the healthy interval starts only after binding. `requested=written=360`, `visible_events>=3`, `mask_paired=mask_required`, `unpaired_dropped=0`, `accepted_ambiguous=wrong_view_count=missing_masks=label_identity_errors=0`, both absolute edge errors **0**, `edge_events_checked>=3`. Owner inspects at least **20** distributed written pictures (`wrong_view_checked>=20`) plus the three edge pairs. Rejected-other-world/context-null counts can legitimately be nonzero; they are diagnostics, not failures by themselves. Case 13 must actually exercise non-owner rendering. Any fallback acceptance requires the uniquely bound discovery path and its home proof, not acceptance by game show flags alone.

If a case writes zero/fewer frames, transcribe this supplementary **I-failure form** from the same receipt, again numbers only:

```text
host receipt case sampled colour_served mask_served owner_scene_unknown owner_target_unknown scene_mismatch target_mismatch not_game_view context_world_null ambiguous_owner owner_render_missing stale_generation pending_at_stop
```

These counters distinguish the hypotheses that the original office report could not. They are callback/request counts as explicitly headed, not additive denominators of one another. The owner does not need to diagnose which C++ branch failed. There is no “try the old frame-key path” automatic office fallback.

**Checkpoint N after J11: prove the pilots on actual content.** Per host run case **21** MISS-1 targeted, **22** MISS-1 auto, **23** MISS-2 targeted, **24** MISS-2 auto, initially **1,800 requested samples at 30 Hz** with a static/near-static view. This is a trial duration, not an assumed success rate. Use normal settings first; record the actual AA method. A separate owner-approved AA-off/FXAA diagnostic may explain temporal uncertainty, but may not erase a failing normal-settings result. Every certifying case needs ≥3 measured aimed successes; only predeclared genuinely supported targets can justify a targeted supported-scope claim. Auto zero eligibility on a VT-heavy view remains a limitation, even if MISS-1 works.

The exact **N form**:

```text
host receipt case aa_method elapsed_ms requested written candidates eligible attempts armed aimed_successes no_effect ambiguous unknown missed_aim cancelled pending effect_events collateral_events excluded_samples failed_samples counted_without_mask restore_unverified lost_ownership wrong_object_count unflagged_changed_samples edge_events_checked max_abs_onset_error max_abs_offset_error
```

Readings come from the lifecycle counters, local checker and owner edge/target inspection. Required: `eligible>0`, `aimed_successes>=3`, `pending=failed_samples=counted_without_mask=restore_unverified=lost_ownership=wrong_object_count=unflagged_changed_samples=0`, both edge errors **0**, `edge_events_checked>=3`; the outcome equation in §B5 reconciles, and `requested=written+excluded_samples+failed_samples`. No-effect attempts are permitted and must lower yield rather than inflate successes. Ambiguous attempts can occur only with recorded exclusions and verified recovery; unknown attempts are reported too, with their exact exclusion/failure cause, and any failed payload prevents a clean certifying leg. Report these rates, do not suppress them. A lower written count is explained by exactly the excluded/failed source samples, not by plausible totals. The home already-default and no-op controls are mandatory before this office step; the owner is not asked to create test assets or enable bench levers in an office host.

The numeric reader also prints a compact exclusion denominator by reason when needed: `host receipt case vt_bindings ordinary_colour_bindings host_mid_candidates host_mid_identity_pass section_footprint_unmeasured no_fresh_census control_disturbed`. These are counts, not scene data. Chat uses them to distinguish a real scope/yield limitation from an F1 outage. Owner time is roughly two short identity/calibration visits, dependent on existing office setup and target availability; schedule is the owner's call. These readings establish the tested office configurations, not universal support for every viewport or material.

## G. Dependencies, risks and decisions

**Dependencies:** none new proposed. Injector already has Core/CoreUObject/Engine/InputCore and private Foliage/RenderCore/RHI/AnomalyShaders (`I/AnomalyInjector.Build.cs:9–22`). Capture already uses AnomalyInjector/ImageWrapper/Json, non-Shipping RenderCore/RHI/Renderer/Slate/SlateCore/ApplicationCore/AnomalyShaders and editor-only UnrealEd (`C/AnomalyCapture.Build.cs:326–365`). Keep probe implementation in Capture, transaction/levers in Injector and the provider interface in Injector's public types, so there is no reverse dependency or module cycle. Keep the existing renderer-private include footprint; no new private header, engine edit, host-game module, MeshDescription module, shader hook or new dashboard package is justified by these two pilots. If linkage to a proposed public API exposes a missing dependency, name the precise module/symbol and return to chat before adding it.

The principal remaining risks and proposed disposition are:

| Risk / open question | Proposed handling and decision owner |
|---|---|
| F1 backup is incomplete and office root cause not identified | Choose (a), finish and prove J2, then obtain I readings. **Chat** rules on the base/identity design. No unsupported claim that null context was the office cause. |
| Owner target genuinely cannot be resolved, or same-scene multiple game views | Bind only after uniqueness proof or refuse loudly. No first-family-wins fallback. Split-screen/stereo support is not included without a separate measured design. **Chat** decides any expansion. |
| A publication barrier touches the capture core and public frame mapping | J5 owns the source/published split and atomic artifact reconciliation. Excluded samples must never become clean published frames or shift annotation vs video silently. **Chat/owner** approve the explicit shorter-clip behavior in this plan; no legacy key is redefined. |
| Thresholds, budgets and office yield are unknown | Five-clean/four-observe/two-recovery, 2 s watchdog, visible threshold and retention limits are starting experiments. Freeze before legs; revise only by dated predictions and measured justification. **Chat** rules on acceptance; **owner** decides whether measured yield is useful. |
| Component masks do not identify arbitrary material sections; opacity/WPO/receiver effects can escape the footprint | Keep declared v1 scope and counted exclusions; do not call selection preferences universal incompatibilities. A section-mask/receiver-mask extension would be a new plan. **Chat** reviews this measurement boundary. |
| Sibling MID differs in a layered/host-specific property, or gameplay modifies it | Full-identity parameter checks plus pre-arm unchanged-image control; cancel on host mutation, restore original current pointer. No unsupported MID parent and no blanket ban on stable host MIDs. |
| Compare-and-restore cannot detect an identical intervening host write | Public pointer/value snapshots prove owned-state equality only under the observed stable-host condition; identical writes are not universally traceable without instrumentation. Declare this limit, exercise detectable takeover, and never advertise a universal transaction over host gameplay. RM restores owned material state and observed appearance, not elapsed physics, streaming-cache history or temporal history. |
| Material state is not a per-draw shader receipt; picture change may be scene noise | R4's three independent evidence legs, stable owner/geometry, clean controls and passive RT readers; no per-draw claim and no global shader-completeness veto. |
| m26, write failures, tag collisions or teardown can erase evidence | Pre-publication mask/write requirements, final event/cause reconciliation, bounded quarantine/retention and explicit failure counters. A later contradiction blocks qualification, not just one JSON key. |
| Optional provenance scope/wording | **Chat** approves nested v1 shapes and the explicit controlled MISS-1 record in `natural_causes`; **owner** chooses UI A/B and tells client before first natural shipment. Ordinary no-effect attempts stay diagnostics only. |
| Legacy F3 packed-channel/normal no-effect and F5 copy churn remain on controlled routes | The new natural attempt path solves its own observation problem. Do not claim a wholesale shipped-anomaly onset-gate retrofit. Carry limitations and run controlled regressions; a broader retrofit is a separate scope decision. |

BLUR-2 remains research only; MISS-3 is dropped; no motion-dependent cause is scheduled. GONE-1 still needs R5's reference-mask proof; BLUR-1 still needs every-visible-user accounting and bounded R6 repair. Later NRM-3 uses `/Engine/EngineMaterials/FlatNormal` with an explicit plugin cook reference. The approved corrections about PIE usage flags, VT decode pink, reduced-mip proxy blur and unsupported menu entries are preserved; none is “fixed” by changing this report's labels.

**Disposition:** the file-level plan is ready for Code verification and chat's ruling, followed by the owner's one go/no-go. **Chat review REQUIRED for implementing this plan and for the final correctness/source/evidence disposition before merge or release.** No implementation, build, cook, launch, capture, staging, process change or repository mutation was performed. Only this run-folder report was written. The frozen release remains the delivered artifact.

VERDICT: PLAN READY FOR CHAT/OWNER APPROVAL | base=d7eee97 + six 091-05 implementation commits + completed 091-06 F1 | identity=a | jobs=11 (40–62 h) | office readings=I:11–14 after J2, N:21–24 after J11, per host, numbers only
