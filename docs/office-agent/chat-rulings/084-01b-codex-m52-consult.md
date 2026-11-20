<!-- relay: run folder runs\2026-09-28-084-01b-m52-consult | model gpt-6-astra | effort max | duration 1044 s (13:38:10-13:55:34) | collected by Claude Code 084-01, content unedited -->
**084-01b — independent m52 reconciliation, 2026-09-28**

The two root defects remain established. I agree with retaining the event and using a receipt from its captured render. Corrections are needed to the claimed minimum lag, the bank-wide phase-lock explanation, the visibility gate, and timeout/finalization behavior. I also correct my earlier implication that F-H frame 171 establishes complete visual recovery.

This was read-only except for this report and analysis artifacts in the relay run directory. No builds, launches, captures, staging, repository writes, or git-state changes occurred. `master` and `origin/master` both resolve to `b5f15a3bf4f4db5a80261c7d3e62e9a723afba8e`. The historical bank demonstrates failure shapes, not qualification of that commit or an office reproduction.

**Citation keys.** Plugin citations below are to `git show master:<path>` in `D:/IntrusiveAnomalies/_wt_084_01_m52`, not its uncommitted working files:

| Key | Path |
|---|---|
| `Mip` | `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` |
| `Capture` | `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` |
| `SVE` | `Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp` |
| `Writer` | `Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp` |
| `Injector` | `Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp` |
| `Diag` | `D:/IntrusiveAnomalies/_wt_084_01_m52/docs/sessions/2026-09-28-084-01-code-m52-label-timing-diagnosis.md` |
| `Measure` | `D:/IntrusiveAnomalies/_reviews/084-01-code-m52-measure/` |
| `UE/` | `D:/UESource/UnrealEngine/Engine/Source/Runtime/` |
| `F/` | `D:/IntrusiveAnomalies/_bench_sessions_bank/M52F_B_GC_GATED_NAT_try1/session_20260920-225013/` |
| `N/` | `D:/IntrusiveAnomalies/_bench_sessions_bank/M52C_M52C_G6_M52_NOPACE/session_20260920-203634/` |

`si` is the saved `session_index` and PNG number; `gfc` is its `labels.jsonl.frame_index`. They are not interchangeable. Rows 1–4, 9 and 10 retain their substantive conclusions; the exact “static-camera” census supporting row 4 needs the measurement correction below.

**5 — CANNOT-DECIDE the literal last-visible frame or uniquely attribute the residual. The half-depth arithmetic supports Code, and my frame-171 wording needs correction.**

I reproduced the archived event `(sid=session_20260920-225013, lab0=159, lab1=165)` using its 34,922-pixel mask union and the archived gradient calculation (`Measure/measure.py:76–116`, `measure2.py:65–83`). Here are the relevant values from the PNGs; they agree with the rounded `S_series`/`gfc_series`:

| si | gfc | S / pre |
|---:|---:|---:|
| 165 | 188 | 0.50249 |
| 170 | 195 | 0.61813 |
| 171 | 198 | 0.74555 |
| 172 | 199 | 0.77462 |
| 175 | 202 | 0.84309 |
| 180 | 207 | 0.90915 |
| 192 | 219 | 0.96583 |
| 193 | 220 | 0.95663 |

`pre=5.2687769`, median labelled `held=2.7984648`, so the half-depth threshold is **0.7655706 × pre**. **For Code's half-depth gate, the last positive is si=171 and the first clean is si=172:** six unlabelled saved pictures, 166–171, remain below that threshold. “50% blurred” should mean “at least half of the measured gradient deficit,” not a calibrated perceptual blur percentage. Enlarged, identically scaled interior crops of `F/Actual_Frames/frame_00139.png`, `00151`, `00170`, `00171`, `00172`, `00180`, `00192` and `00200` show residual softness at 171. “Detailed again” was a qualitative recovery observation, not proof that every visible effect ended at 170.

The archive's `first_full_back=192` is also not a stable visual endpoint. Its noise-band threshold is **0.9651382 × pre**; 192 barely crosses it, and 193–199 fall below it again. `measure2.py:83` selects the *first* crossing without requiring persistence. No three consecutive clean saved frames satisfy that threshold between 171 and the next same-target Apply at si=201.

For attribution, **temporal history is a plausible leading explanation, not demonstrated TAA causation**. All three event textures (`T_rock_02_AORM`, `_D`, `_N`) have successful GT restore acknowledgments at gfc=196, before si=171/gfc=198 (`F/anomaly_log.txt:2779,2781,2783`). Successful completion follows the RT update (`UE/Engine/Private/Streaming/Texture2DUpdate.cpp:147–156`; `UE/Engine/Private/StreamableRenderAsset.cpp:174–186`). Thus a still-unfinished original restoration of one of those three textures does not explain the post-171 creep. There is no trace excluding subsequent resource changes. The next fire starts on another target/texture at si=170/gfc=195 (`F/anomaly_log.txt:2752`; that row's `stuck_mip.textures` names `T_Grid_A`). Moving illumination, other temporal rendering, and scene drift remain confounders; this run has no matched long no-hold sequence or AA ablation. A history-mediated residual can itself be a visible anomaly: those categories are not mutually exclusive.

**Proposed predeclared criterion:** retain half-depth as a named *dominant-step diagnostic*, giving last-positive **171** here. A gate advertised as *last visible blur* should instead use a fixed interior ROI, matched same-view null pictures, and a significance threshold calibrated on independent nulls before inspecting candidate edges—for example, gradient deficit greater than `max(5σ_null, 0.02 × S_null_baseline)`. Define the last positive immediately before three consecutive clean, continuously captured render frames; confirmation must not shift the classified edge. Require a stable negative suffix or report an unresolved/censored edge. ROI, reference, steady held depth if used, and search interval must not be recomputed from candidate labels. Under that stronger contract, **this bank does not certify a single last-visible frame**; neither 170 nor 191 is justified as such.

Reproduction artifacts beside this report: `consult_pixels.py`, `F-H-event3-recheck.json`, `F-H-event3-detail.png`, and `F-H-event3-crops.png`.

**6 — AGREE with withdrawing `MipBiasFade` as the sampling-fade explanation.**

The inspected runtime source has the `CalcMipBias()` call in `UE/RenderCore/Private/RenderResource.cpp:1096`, inside bookkeeping for `SetNewMipCount`; no renderer consumer was found. Small citation correction: `UE/Engine/Private/Texture.cpp:1128` calls **`MipBiasFade.IsFading()`**, not `CalcMipBias()`. Neither establishes a mip-sampling fade. The withdrawal at `Diag:97–101` is warranted; adding the fade value to a render receipt would not explain or measure the residual in row 5.

**7 — DISAGREE with a guaranteed ≥1 captured-frame onset lag, even excluding synchronous flushes.**

The source proves **RT update before successful CPU acknowledgment**, not one intervening captured view. Here is an allowed ordinary asynchronous ordering:

1. Tick-end sample F sees baseline; scene F is queued before its streamer tick (`UE/Engine/Private/GameEngine.cpp:1775,1891,1901`).
2. That streamer tick schedules `Finalize` behind scene F. After the update tick unlocks, the RT executes scene F using the old resource, then finishes the swap.
3. Before the CPU executes its following `IsCompleted()` test, the RT has finished; that same streaming poll updates the mirror. The next sample and render, F+1, both see low mips. There was no earlier low-mip captured picture.

There is no frame fence prohibiting step 3: `FRenderAssetUpdate::Tick` releases its lock at `UE/Engine/Private/Streaming/RenderAssetUpdate.cpp:203`; RT scheduling is at `:267–277`; `UStreamableRenderAsset::TickStreaming` calls `Tick` and then checks completion at `UE/Engine/Private/StreamableRenderAsset.cpp:172–178`; `IsCompleted()` tests only `TaskState == TS_Done` (`UE/Engine/Public/RenderAssetUpdate.h:61–65`). A CPU scheduling pause suffices; a flush is unnecessary. Capture gaps/drops independently prevent turning a render-frame separation into a universal saved-frame bound.

The useful statement is: **the mirror can lag rendered residency, and the bank demonstrates that it does.** The observed one-frame dominant steps remain evidence. “Always ≥1” and a general `1…~5` range (`Diag:13–14,65–74`) are not correctness guarantees.

**8 — DISAGREE with the universal and causal claims; staged polling remains a plausible contributor.**

The final-stage in-flight poll exists (`UE/Engine/Private/Streaming/StreamingManagerTexture.cpp:1740–1744`), but simultaneous sampled counts do not uniquely identify that poll or reveal each texture's RT transition. Multiple assets can share a slice, and a multistep aggregate image signal can include temporal processing. No per-texture RT trace was archived.

There are direct counterexamples to both universal bank claims:

- **Counts do not always flip together.** `N/labels.jsonl:46` (si=45/gfc=56) has four textures at 7, but `_D` and `_AORM` still at baseline 11. At `:52` (si=51/gfc=62), those last two become 7. Their restore acknowledgments are at revert+13 (`N/anomaly_log.txt:698–699`), while the other four acknowledge at +7 (`:661–664`). My archived-series audit finds simultaneous first cached drops in 65/70 measurable records, with five exceptions including textures that never drop during the recorded event. “All held textures, every measured event” (`Diag:148–152`) is false.
- **The cited F run is not uniformly 19/35.** Its early Applies at gfc=20,55,90,… do have the repeated 35-frame cadence. Later, Apply at 451 opens at 469 after **15** pre-roll frames (`F/anomaly_log.txt:5966,6237`); Apply at 577 opens at 595 after **15** (`:7436,7665`); other windows use 17 (`:4089,8264`). This contradicts `Diag:156–159`. The seven-stage cycle is nominal: the final stage waits for `AsyncWork->IsDone()` (`StreamingManagerTexture.cpp:1736`), and the burst FSM itself waits for acknowledgment. Constant phase can be an outcome of that feedback, not an independently established cause.

Consequently, B1's “unchanged ⇒ slice mechanism refuted” and B2's promised disappearance of constancy are too strong (`Diag:195–196`). Record actual processing stage, asset slice, RT transition and acknowledgment identity; changing one scheduling cvar need not change a bottleneck elsewhere. Also, the overlapped slice task is dispatched to `AnyHiPriThreadHiPriTask`, tagged as parallel game-thread work—not a deferred task executing on the game thread (`StreamingManagerTexture.cpp:1608–1619`; correct `Diag:73–74`). No constant edge shift is supported.

**11 — AGREE with a restore-settle gate for this narrowly scoped draft; it is not a universal requirement for correct labels.**

Overlap is acceptable *if* the implementation owns independent event records, frozen baselines and texture sets, stable mask tags, and conflict handling for shared textures. The proposed unkeyed `GetRenderTruthTextures()` over `Held + Restoring` does not provide that isolation. Today there is one anomaly instance per ID (`Injector:172–175,705–721`); Apply resets `Held` (`Mip:362–365`), while `Restoring` persists. Allowing another fire without a per-event design risks assigning the new hold to the old tail or vice versa. Existing refusal of a reused restoring texture (`Mip:334–359`) helps but does not establish multi-event ownership.

For the smaller change, a gate is the defensible choice. **The gate must keep capturing while it waits.** The existing `SettleAfterRevert` phase does not (`Capture:762–768`). Event retirement also needs a restoration-generation acknowledgment and no pending anomaly-owned transition; an old baseline receipt, or baseline while a stream-out is still pending, cannot retire the event. This preserves the already-back race correction at `Mip:712–731`.

The timeout treatment in `Diag:218,224–225` is unacceptable as a clean-label boundary. **A bounded wait may end the run/refuse further fires with an explicit unresolved outcome; it cannot silently stop labelling pictures that remain anomalous.** The existing timeout deliberately continues tracking (`Mip:849–858`). Logging a cap does not make the resulting false negatives correct. Teardown, target loss, host takeover and capture cutoff likewise need distinct terminal outcomes.

**12 — AGREE that the captured family's SVE callback can latch ordinary rendered residency, with concrete qualifications.**

For the inspected conventional texture path, scene rendering and RDG execution occur within the scene render command (`UE/Renderer/Private/SceneRendering.cpp:4650–4667,4387–4392`); streaming finalization is a separate RT command. Reading in `AfterPass_RenderThread` for the matched family (`SVE:143–184`), before submitting that request's result (`:235–237`), is therefore a valid boundary for the ordinary resource state used by this render. `GetState()` explicitly supplies RT-coherent state (`UE/Engine/Public/Rendering/StreamableTextureResource.h:37–42`). This means a read in the RT callback, not an arbitrary deferred/parallel pass lambda or GPU-completion poll.

The design still needs these safeguards:

- Resolve the resource **on the RT** from the retained object and record/validate its generation against the baseline. Establish that baseline from a pre-Apply render receipt or demonstrated idle/coherent state; another potentially stale GT count is insufficient. `UTexture::GetResource()` selects an RT-specific pointer (`UE/Engine/Private/Texture.cpp:176–184`); retaining the UObject does not prevent resource replacement/deletion (`:206–242`). Null, uninitialized, replaced or unsupported resources require an explicit unknown outcome, not numeric zero or assumed restoration.
- Bind the immutable receipt to run epoch, request, owning scene/target, actual family/view and event; join RGB, masks and receipt on those identities. Arrival order and `GFrameCounter` are insufficient. Missing or stale receipts must not fall back to the GT mirror.
- The partially resident path is not finished merely by writing “read first visible mip.” Visibility is changed by `RHIVirtualTextureSetFirstMipVisible` (`UE/Engine/Private/Streaming/Texture2DStreamOut_Virtual.cpp:35–43`; `Texture2DStreamIn_IO_Virtual.cpp:53–58`). The inspected generic RHI API supplies setters (`UE/RHI/Public/DynamicRHI.h:983,991`), not the proposed getter; `GetCurrentFirstMip()` only derives from the same resource state (`StreamableTextureResource.h:69–72`). That path needs a verified transition record/platform mechanism or an explicit unsupported disposition.

This is a receipt for **available mip state**, not sampled mip, material contribution, or final-RGB history. Code acknowledges that at `Diag:222–224`; the acceptance gates must honor the distinction.

**13 — DISAGREE that Code's proposed gates are sufficient.**

Keep both old-behavior levers and B5. Add the following tests from my original proposal:

1. **Independent metadata corruptions:** onset +2 pictures and offset −5 pictures, moving label and mask membership together while preserving RGB and the previously frozen pixel oracle. Both must fail. These can be offline transformations, not extra game launches. The archived measurement currently uses candidate labelled frames to estimate held depth (`Measure/measure2.py:69–78`) and starts its offset search after the candidate label end (`:80–83`); it is not yet an independent acceptance oracle.
2. **Controlled delayed restoration:** hold one visibly contributing texture low for at least five captured frames after revert, including a two-texture case where the other has restored. Prove per-event ANY-on/ALL-restored-off behavior.
3. **Controlled delayed GT acknowledgment with unchanged render transitions:** receipt-derived labels/masks must stay with the render transitions. This directly tests removal of mirror dependence.

Also qualify missing/out-of-order receipts, resource replacement, early revert with pending stream-out, timeout, and both capture orders. These exercise the new lifetime/join mechanism, rather than merely showing that the old implementation fails.

As written, E-ONSET/E-OFFSET demand exact equality between an ANY/ALL-residency window and an aggregate half-depth pixel window (`Diag:218,228–233`). That equality need not hold: a weakly contributing first texture can precede the dominant step; temporal history can outlive the final restoration. A controlled single-texture, stable-detail case without temporal accumulation can test basic equality. Staggered textures need their own witnesses, and the production temporal path needs a separate final-RGB criterion. Otherwise a correct residency implementation can fail this gate, or a wrong visibility claim can pass it.

**Ranked corrections and remaining bench-plan defects.** These are proposed dispositions for Chat, not implementation orders.

- **P1 — Fix contract and terminal behavior:** resolve the residency-versus-visible-edge mismatch (rows 5, 12, 13); prevent timeout from manufacturing clean frames (row 11); preserve event/resource identity through late completion. The drain-time change also must consistently update authoritative membership/telemetry. Merely changing `FireLabelled` and adding `rendered_resident_mips` leaves `stuck_mip.held` sourced from the old GT mirror (`Mip:928–959`; `Capture:4890–4906`; `Writer:133–146`). It also leaves the sampled `ConditionHeld` veto in `observable` (`Capture:4882–4887,4057–4072`; default delegation in `Source/AnomalyInjector/Public/IAnomaly.h:32`). The design needs one explicit authority for the effective window, with GT diagnostics distinguished from it.
- **P2 — Repair the measurement census:** `Measure/measure2.py:14` reads `view.rotation`, but the schema writes `view.rot` (`Writer:220–221`). Line 19 measures the largest *adjacent coordinate step*, not total camera excursion. For example, N's event starting at si=45 passes its reported 3.4567-cm check although Apply-to-last-label displacement is **20.121 cm**. `measure2.py:96–100` also selects the first verification line within 200 modulo ticks without joining the texture/event or requiring ALL restored; N's +13 completion demonstrates the consequence. Recompute the camera-qualified denominators and restoration summaries before using “59 static,” “55 complete,” or “55/55 +7” as acceptance premises. The clear F event examined above is not invalidated. `audit_series.py` and `series-audit.json` archive this check.
- **P2 — Make §7 executable as claimed:** B0's `2 4 8 30 0` still omits both settle phases (`Capture:722–724,762–764`); a long post-gap does not capture every frame. Even K=0 does not itself add a capture in those transition branches. With the assumed 19-frame pre-roll, approximately `19+8+30=57` saved frames per burst gives roughly ten complete bursts in 600 frames, not ≥15. Size by completed measurable events. B4's cvars throttle manager-issued load requests (`UE/Engine/Private/Streaming/StreamingManagerTexture.cpp:1341–1367`), whereas m52 directly calls `StreamIn` (`Mip:715,844`); they are a stress experiment, not a guaranteed restore-delay lever. B6 timeout=3 likewise needs proof that a stream-out is actually pending, or it may only cancel before any request exists. B1/B2 need the trace-based interpretation in row 8, and B5 must validate the entire comparison window, not just nine early frames.
- **P3 — Wording/provenance:** distinguish half-depth recovery from last visible; first verification from all-texture verification; and the synchronous polling mirror from the thread on which overlapped updates execute. Correct the `IsFading` citation. Preserve the pre-reconciliation findings note as historical evidence of the withdrawn explanation, not current support for it.

**Chat review REQUIRED** for the visibility/residency contract, timeout disposition, and implementation/requalification decision. The read-only reconciliation is complete; no fix or bench execution is authorized by this report.

VERDICT: AGREED-WITH-CORRECTIONS
