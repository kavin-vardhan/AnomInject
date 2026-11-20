# 081-42 — Independent review of Code-authored m55 changes

Reviewer: Codex. Date: 2026-09-26. Request: [081-42-codex-review-request.md](D:/IntrusiveAnomalies/_reviews/081-42-codex-review-request.md).

**Disposition: CHANGES REQUIRED.** Six P2 findings need resolution before merge. I found no new P1 or P2 defect in the build 2/3 C++ changes. The material problems are validation gaps and client documentation, not a demonstrated failure of the banked producer output.

**Chat review REQUIRED for disposition and the next implementation brief, before merge:** F1 and F2 expose ways the current gates accept a regression; F2 also challenges an explicitly ruled acceptance policy. This review does not authorize a merge, new capture, build, or repository edit. Code retains implementation ownership.

**Scope and state.** I read the startup handoff, operating contract, review request, 081-20 independent report, relevant subsequent rulings, both Stage 3 reports, predictions/amendments, source changes and banked artifacts. I ignored the scorecard.

- Repository: D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector.
- Reviewed feature and origin: eafa6544b4182d5a5a869a9a788ff7284d4495c8.
- Live checkout remained m51, 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67.
- Master remained 031a103b18b90b2ce9dd626b22e9498717c60815.
- Source scope: 9c75abe and 85c99b3; oracle scope: ea9946a and c27024d; documentation scope: eafa654. There is no Source diff between 85c99b3 and eafa654. Build 3 remains the reported Game 002805CF build.
- The 15 harness files hashed in the cost boundary matched. The final external 081-23-lib.py SHA-256 was f5876b0545f9ab55453c5c4e86fe57f2c6b27370d26f394b31c88f95cfd71bcb.

Repository file citations below use Git object notation, for example eafa654:tools/verify_capture.py:2529. They refer to that revision in the repository above, **not the different live m51 file contents**. External harness links refer to the live, hash-checked files.

All probes were read-only, with mutations confined to memory. No game, editor, build, capture runner, staging command or comment-stripper write operation ran. The requested review report is the only file written. Existing untracked repository files were preserved.

## Findings requiring resolution

**F1 — P2 — A contradicted empty_region refusal can pass the full G270 gate. R2/R4.**

Files: eafa654:tools/verify_capture.py:2441, :2494, :2529; [081-23-lib.py:793](D:/IntrusiveAnomalies/_reviews/081-23-lib.py:793), :817 and :823.

The oracle detects a delivered mask with positive target and control denominators, prints DISAGREES for the empty_region refusal, and increments empty_region.disagrees. Its return code considers only measured-row and reference mismatches. G270 accepts an empty_region reason as an accounted required pair and checks the oracle's exit code and measured-row mismatch count; it never consumes that disagreement.

I reproduced this through the actual oracle, 081-08 audit and final lyra_g270_eval, using the real bank:

D:/IntrusiveAnomalies/_bench_sessions_bank/M55B3R23_C3_LYRA_G270_N_A1/session_20260926-041724.

The unchanged bank replayed PASS: 28 compared/matched rows, nine events, seven events with measured pairs, four existing empty_region refusals. In memory, I changed only window 3 at session_index 21, stuck_low_mip@4064, phase 0, to an empty_region refusal with unavailable target/reference statistics, and adjusted its phase accounting and measurement counters. Delivered images, mask, frame identities and other pairs remained unchanged.

Result: audit errors [], oracle exit 0, 27/27 measured rows match, empty_region.disagrees = 1, and **G270 still PASS**. The mask actually contains 184,933 target pixels and 1,888,667 control pixels. Thus the “honest refusal” clause can hide a lost required measurement even with the measured-event floor satisfied.

Fix: propagate a proved empty-region contradiction into a failing result. Prefer oracle exit 1 for this comparison mismatch, with a dedicated summary count, and require G270 to reject it. Add an integration negative test using this case. Keep missing/all-zero-mask PNGs distinct as unverifiable; do not invent a disagreement merely because no PNG was written. The four original refusals were unverifiable from delivered mask PNGs, not disagreements.

**F2 — P2 — Synthetic-order self-excusal can hide a new, internally wrong legacy count. R4.**

Files: [081-23-lib.py:656](D:/IntrusiveAnomalies/_reviews/081-23-lib.py:656), :660–674, :704–708; missing per-label count cross-check at :359–366.

Both the unstable field set and run-unique paths include the candidate's own ON/ON disagreement. In synthetic order those same sets exempt cross-binary differences. G297 reports a singleton, but only recurrence in at least two legacy groups without pre-m55 recurrence changes the disposition.

Concrete bank-based mutation: the four MAIN-S legs replay PASS. In MAINON_S2, changing labels/4/anomalies[0]/target_pixels from 20,949 to 20,950, and changing the corresponding derived reading counts entry, still returns PASS, problems [], and no tripwire. This path is neither historical nor timing-sampled. It is reported as a self-excused build3-ON singleton. The delivered mask, other legs and all oracle data are unchanged. The existing own-tag check verifies mask membership and equality of the two counts in the MASK-TIE log; it does not tie this serialized label count to the actual mask.

This follows the ruled algorithm; it is a weakness in the acceptance rule rather than an implementation departure. I agree that two observations cannot establish the cause of every deviation. I do not agree that this makes a candidate-only violation of an independently checkable invariant safe to PASS.

Fix: enforce per-leg invariants before noise excusals, including serialized target_pixels versus the decoded count for that row's tag. For new non-timing candidate-only differences that cannot be resolved from artifacts, use NEEDS-DECISION rather than learning an exemption from the candidate's own pair. Keep the frozen historical/timing exclusions and recurrence report as separate, visible limitations. At minimum, the concrete corrupted-count case must fail.

This does not establish that MAINON_S2's original data were wrong. The actual MAIN-S singletons replayed from raw banks were coverage_pct fields, already classed as timing-sampled, one on pre-m55 and one on build 3.

**F3 — P2 — Corrupt oracle inputs do not have a reliable 0/1/3 classification. R2.**

Files: eafa654:tools/verify_capture.py:2282, :2409–2417, :2529; eafa654:docs/change-evidence-fields.md:278–282.

Malformed JSON and non-object JSON lines are counted and skipped, but do not affect the exit code. A sidecar containing only "{bad json" returns 0 with zero rows and one unparseable line. A valid measured row followed by that malformed line returns 0 with one match and one unparseable line. This loses transport data while presenting the same success status as a completely readable input.

A separate type failure makes the exit taxonomy ambiguous: a valid JSON row with receipt = [1] reaches .get("rect") on a list and raises an uncaught AttributeError. The CLI then fails through Python's exception exit rather than the documented structured “cannot run” result. receipt = [] instead becomes an unavailable comparison and exits 0. Neither case produces an arithmetic mismatch.

Fix: classify malformed serialization and invalid required container types explicitly, with line/row diagnostics and a structured non-success result. Exit 3 is appropriate for input the oracle cannot interpret; specify precedence if readable rows also mismatch. Catch decode/type errors at the input boundary. Preserve the intentional exit 0 for a **well-formed** session with no comparisons; that approved coverage rule is not this finding. Add tests for malformed-only, mixed valid/malformed, and invalid receipt types.

**F4 — P2 — The README states the opposite of what the control region includes. R3.**

File: eafa654:docs/client-readme.md:1030–1033.

The sentence saying the control does not include light or shadow cast by the target is false. Source AnomalyChangeMeasurement.cpp:228–229 accumulates pixels by the current mask byte; :257 emits Stats[0] as the control. Light, shadow and reflection changes on untagged surrounding pixels therefore contribute. The field reference correctly explains this at :57–60.

Failure scenario: a target changes its shadow on an untagged floor. A reader following the README treats the busy control as unrelated background disturbance even though it contains the anomaly's own effect.

Fix: say the control excludes tagged target pixels but can include light/shadow/reflection spill onto the surroundings. Match the field reference, and retain the warning against automatic subtraction or causal inference.

**F5 — P2 — Refusal rows are incorrectly documented as containing only -1/null numbers. R3.**

Files: eafa654:docs/change-evidence-fields.md:18–19; eafa654:docs/client-readme.md:993–994.

An empty-target refusal can retain valid control statistics, a reference index, camera/identity bookkeeping and earlier phase maxima. Source AnomalyChangeMeasurement.cpp:256–261 and :286 emits these intentionally; the field reference itself acknowledges the control exception at :71–73.

An actual counterexample is:

D:/IntrusiveAnomalies/_bench_sessions_bank/M55B3R23_C2_EMPTY_N_A1/session_20260926-035923,
session_index 3, missing_object@1725: reason empty_region, chg_n = -1, ctl_n = 921600, ctl_gt8 = 4579, ref_session_index = 2.

A client implementing the introductory rule can reject a correct row or discard valid control evidence.

Fix: qualify the rule as unavailable measurement statistics using sentinels. Explicitly retain valid control statistics on an empty-target refusal and distinguish row bookkeeping/running maxima from the unavailable target/reference measurement. Audit the similar prose against the field tables.

**F6 — P2 — Cost and full-yield observations are presented as general guarantees. R3/R5.**

Files: eafa654:docs/client-readme.md:1098–1101; eafa654:docs/change-evidence-fields.md:259–266.

The statements “no measurable cost,” unchanged writer throughput, no dropped frames and every required pair measured omit the host, recipe, observation window and measured quantity. The bank supports these observations for a particular paced fixture; it does not establish them for every 720p/1080p paced capture. “Every required pair” also conflicts with the correctly stated absence of a structural full-yield guarantee in the memory section.

The headline used the same build OFF/ON, a static StackOBot fixture, native order, solid_swap, Config 2 4 16 4 0, 600-frame legs and steady indices 60–599. Each ON leg measured 120 required pairs, not a dense workload measuring every captured frame. CPU-cycle measurements exclude blocked wait. Different phase density, scene/writer load or a blocked game-thread operation can invalidate the broader reading while leaving this benchmark unchanged.

Fix: frame this as a dated result on the tested fixture and machine. Name the statistic as the one-sided 95% upper bound on the **mean game-thread CPU-cycle-equivalent delta per engine frame**, report -0.0718/+0.1100 ms, and state that the eight ON legs measured 120/120 each with zero drops. Say that throughput showed no detected loss on that load. Retain the pacing-off stress failure and the absence of a universal memory/yield guarantee. The measured result need not be discarded or rerun merely to correct this wording.

## Answers and remaining notes by question

**R1 — Build 2/3 source: no new P1/P2 finding.**

I traced the changed code through issue/arm, family attachment, readback, writer completion, payload admission, worker measurement, phase-reference retention, cancellation and closure.

| Requirement | Review result |
|---|---|
| 256 MiB admission | AnomalyChangeStage.cpp:168–172 checks requested bytes against MaxBytes - BytesHeld under CS. Colour and mask reservations precede taking stage ownership (:328–358). A failed admission records budget_exceeded; it does not wait for capacity. This is a retained-payload cap, not process memory or a count-based bound. |
| Ownership census | :182–257 counts unique colour allocations through ColourOwners, partitions aliases once, and counts masks in Pending/Previous/InHand. InHand remains valid across the worker's unlocked pixel loop. Peak/refusal snapshots are made while holding the state lock. No additional payload owner is created by the census. |
| F1 cancellation | MarkSessionBegun is separate from stage construction. StartRun/deinitialization discard a never-begun stage; cancellation discards/resets it before folder cleanup; Persist checks discarded/begun state. I found no new path recreating a cancelled session directory. |
| F2/F3 legacy delivery | SceneViewExtension.cpp:114–128 consumes/publishes the legacy arm independently of successful attachment. Its render callback still submits legacy colour when receipt claiming fails. MaskSceneViewExtension similarly separates bAttachChange from legacy arm servicing and diagnoses unsupported delivery outside its state lock. Multi-view counting is independent of attachment. |
| F6/F7 | Event late_results was removed; the run-level late counter remains. Completion of an issue declared unsupported is separated from a supported late/duplicate completion at ChangeStage.cpp:153 and :308. Epoch rejection still takes precedence. |
| F9 | :82–88 snapshots a nonzero gate back to zero without -IAIBenchFixture and records the refusal. The inspected fault paths use that effective stage gate. |
| Numeric/temporal behavior | Region arithmetic, phase windows, ordered predecessor/reference use, generation rejection and release paths remain consistent with the design. No new leak, double release or receipt mis-stamp was identified in these changes. |

The existing pre-m55 multi-view/key-overwrite behavior is not fixed by restoring legacy behavior. The 081-21 ruling already records it. Likewise the previously reviewed synchronous closure wait, metadata/row limits and worker throughput limitations remain; this review does not turn those accepted limits into performance guarantees.

Independent structural/arithmetic replay across 106 banked build-3 sessions checked 4,380 pair rows, 1,096 event rows and 3,511 measured rows. It found zero violations in the checked ordering, receipt/geometry/token joins, numeric conservation, reference/onset agreement, phase accounting, closure-retained bytes and census partition. This set includes discarded/diagnostic/stress sessions and is **not** a new count of qualified legs. It is not an exhaustive race proof or a fresh runtime requalification.

**R2 — Arithmetic is independently recomputed, but exit 0 is not full sidecar consistency.**

The pixel calculation uses delivered RGB and mask pixels rather than published statistics: maximum absolute RGB-channel difference, strict >8 threshold, mask-tag target and mask-zero control, histogram conservation and four-decimal mean tolerance. Reference comparisons recompute against the named reference PNG. A simple sum corruption produced exit 1 while the unmodified fixture produced exit 0.

F1 and F3 need correction. Separately, the following is a **P3 scope note**, not a request to turn this tool into a renderer-pairing oracle:

At eafa654:tools/verify_capture.py:2248–2337 and :2433–2437, a row with chg_measured true, chg_eligible/pair_valid false, reason budget_exceeded and expected_prev_session_index 71 still matches when its actual named frame pair is 2→3 with correct image arithmetic. The oracle also accepts an onset reference index changed to another identical image, and does not require omitted histogram/sum fields. These were reproduced in memory. It checks neither the complete schema nor phase-reference selection. Thus the direct answer to “can internally inconsistent pairs pass?” is **yes**.

The accepted own-pair-ID limitation is explicitly documented and appropriate to arithmetic-only scope. Keep identity/temporal/schema auditing as a separate required check; add cheap contradictory-field checks if desired, but do not claim they prove renderer pairing.

**R3 — Memory prose is substantially correct; correct F4–F6 and the two notes below.**

The 256 MiB description excludes writer memory and metadata, the 11-image + 9-mask formula is expressly a planning estimate, longer retention is acknowledged, and 1440p/4K are not claimed as measured. I found no restored claim that the formula is a structural bound. The 20/68 MB observations need the workload qualification discussed under R5.

“Refused rather than measured wrongly” is a valid description of the inspected rejection logic. It is not an independent proof that every refusal in every artifact is truthful; F1 shows the current gate's missing check.

**N1 — P3 — Make the delay=3 sentence's scope explicit.** File: eafa654:docs/client-readme.md:1070–1073. The trailing “in both the adjacent pair and ref_*” can attach to the whole sentence, implying both comparisons remain under 0.1% in windows 0–2. The field reference is clearer: that bound applies to adjacent pairs.

The first event in D:/IntrusiveAnomalies/_bench_sessions_bank/M55B3R23_C2_DELAY_N_A1/session_20260926-035712 has target n = 66,837, adjacent gt8 [63, 36, 13, 66837], and reference gt8 [63, 348, 818, 66837]. Reference window 2 is about 1.224%, not under 0.1%. Fix with two sentences: adjacent windows 0–2 were under 0.1%; window 3 reached all target pixels in both comparisons. State that reference values had already accumulated ordinary scene change. The delayed-step and synthetic gradual-ramp distinction is otherwise correct; no runtime gradual-ramp proof is claimed.

**N2 — P3 — teardown_flush is present at zero.** File: eafa654:docs/change-evidence-fields.md:230–232 says change_teardown_flush is absent until it fires. ChangeStage.cpp:78 initializes it to zero and Summary emits counters. A consumer implementing the stated optional-key convention will see a different schema. Remove it from the “absent when zero” list.

**R4 — The amendments implement their rulings, with a material G297 weakness.**

I rebuilt the four c1 legacy comparisons from the raw bank using the final library, rather than relying on cached pre-amendment readings. All four replayed PASS.

| Amendment | What I checked and what the result establishes |
|---|---|
| 1: G8 receipt and resume correction | The undelivered receipt must carry all six delivery-side sentinels; an absent receipt key is not required. Chunk/window runners propagate stop codes and the final library checks recorded failures before resuming. I found no new route that converts a recorded fixture-valid failure into “done.” |
| 2: PQ-A/PQ-B and G270 refusals | Rejection-counter exceptions are restricted to named gates; peak-census counting is per closure/reset with each census checked. G270 accounts for measured pairs plus empty_region refusals and rejects other reasons. F1 is the missing check that an allowed refusal is actually honest. |
| 3: G270 floor | At least three events must each contain a measured required pair for PASS. An all-empty run cannot pass. This floor does not resolve F1. |
| 4: person-present and ENV-VOIDED | The requalification classifier only converts an already invalid foreground-only attempt, with person evidence, to ENV-INTERRUPTED. Valid PASS/FAIL results retain their verdict; a flagged failure stops for decision. NOT-RUN clearing is restricted to a recorded ENV-VOIDED ruling. |
| 5A: coalesced arm | Exact served-arm/unavailable signature, affected unmeasured row, A.6 colour-file dimensions/format, bounded derived-field transformation, one frame/one leg per group and cross-group frequency caps are implemented. Raw replay excused CB_N2 at index 29 only. A.6 deliberately does not compare colour pixels, per the 081-33 correction. |
| 5B: tag bijection | One consistent injective map, background fixed to zero, decoded pixels/per-tag counts, actor identity and serialized map are checked. CB_N5's permutation replayed valid. This permits tag renumbering, not a spatial mask or actor swap under a different map per frame. |
| 5C: historical paths | The historical set is hash-pinned, applied to cross-pair paths, and not grown from this run. It remains a path-level noise allowance, not proof that any changed value on an allowed path is causally harmless. |
| 6: M47 | PREWARM N>=1, per-arm/log consistency, the five-key closed set and frequency caps are enforced. CB_S2 replayed the sole exception: N=1, 90 marked frames. Other fields cannot simply be stripped as M47. |
| 6: G297 | Reporting and recurrence match the ruling. F2 demonstrates why visibility alone is insufficient for the strongest legacy-regression claim. |

A and C also have accepted evidentiary limits: an m55-induced timing change that happens to produce the permitted coalesced signature, or a change on a historically unstable path, can be inside those allowances. The signatures establish the observed mechanism, not its cause. I am not reclassifying the actual banked A/B/M47 exceptions as product defects or reinstating the withdrawn colour-pixel gate.

The answer about the **requalification** person-present gate is no: I found no reclassification of an already fixture-valid result by that gate. Do not transfer that answer to the cost harness: cost-lib.py:412 deliberately excludes person-present attempts before reading cost, even after fixture checks pass. That stricter cost rule was preregistered. All 16 counted paced legs had person_evidence false, so it did not select among their outcomes.

**R5 — The declared numerical gate reproduces; its meaning is narrower than the client prose.**

No arithmetic finding in [081-37-cost-lib.py:272](D:/IntrusiveAnomalies/_reviews/081-37-cost-lib.py:272). The calculation is:

- delta = mean(B) - mean(A);
- pooled variance = (within-A sum of squares + within-B sum of squares) / 6;
- SE = pooled SD * sqrt(1/4 + 1/4);
- upper = delta + 1.943 * SE.

These are leg-level samples, not hundreds of frames treated as independent replicates. The confidence interpretation assumes suitable independent errors and a common within-condition variance; four observations per condition do not establish those assumptions. ABBA balances simple drift, not arbitrary condition-correlated changes.

| Paced result | 720p | 1080p |
|---|---:|---:|
| A mean, cycle-equivalent ms/engine frame | 3.761425 | 6.030075 |
| B mean | 3.646025 | 5.992950 |
| Delta | -0.115400 | -0.037125 |
| SE | 0.0224479 | 0.0757350 |
| One-sided 95% upper | -0.0717837 | +0.1100280 |
| ON measured/required | 120/120 in all four legs | 120/120 in all four legs |
| ON retained-payload peak range, decimal MB | 19.354–20.275 | 56.021–68.469 |

I recomputed all 16 game-thread results from the raw thread-cycle samples, aligning timestamps to labels sorted by session_index, using the frozen 2.688 GHz conversion and the harness's engine-frame interpolation. Every result matched the ledger at its four-decimal precision. All sampled PIDs matched the recorded launched game; all used a GameThread description, with no earliest-thread fallback and no sampler errors.

The writer test implements the declared “no detected drop” rule and uncertainty-width guard. It is not proof of exact equality or unchanged maximum writer capacity: under pacing, delivered throughput is constrained by offered load. Zero drops, complete yield and the recorded latency/backlog behavior support “kept up with this load.” The pacer was configured for 30 fps; observed capture arm rates in these legs were approximately 25.07/s.

**Can a paced PASS hide a real >=1 ms cost? Yes, if “cost” means more than the declared mean CPU-cycle delta.** For example, an added blocking wait can consume pacing headroom without consuming equivalent thread cycles or changing the final paced frame rate. Rare/closure-only latency and denser measurement workloads can also escape this steady-window mean. This is a measurement limitation, not an observed 1 ms regression in the bank.

The G303 replica detected the tested +1.4 ms busy-work additions: +1.2812 ms with tight jitter and +1.2444 ms with wider jitter. Its ~0.2 ms pacer spin stayed roughly constant; the predicted opposite spin movement did not occur and was reported as a miss. This is useful evidence against absorption in that tested regime, not a universal proof that the actual engine pacer cannot mask other costs.

The conversion is host-calibrated, not an API guarantee. Microsoft's [QueryThreadCycleTime documentation](https://learn.microsoft.com/en-us/windows/win32/api/realtimeapiset/nf-realtimeapiset-querythreadcycletime) defines CPU cycles and warns that their timer-rate behavior varies with CPU implementation. The banked duty-cycle and pacer probes support this host's approximate scale. Keep “cycle-equivalent,” calibration and blocked-time exclusion explicit; use an independent elapsed-time instrument if an elapsed-work bound is required. I am not treating the rejected GetThreadTimes sampling as a replacement measurement.

**Is 20/68 MB representative? Only of this fixture/load.** Another build-3 Lyra ON bank reached 134.784 MB; the unpaced 1080p cost leg reached 267.644928 MB with refusals. Retention depends on completion order, writer delay and phase/reference density. The hard 256 MiB admission cap is the bound; neither these observed peaks nor the planning formula is one. No 1440p/4K result is established.

The pacing-off result remains separately **UNRESOLVED at 1080p**, with the original LEG-FAILURE retained. Its 720p PASS was obtained with only 0.17–0.20 measured-pair yield. Making paced capture the headline is a clearly dated change of scope, supported by new full-yield legs, provided those stress failures and limitations remain visible. It does not make the unpaced campaign green.

**R6 — Merge readiness and standing gates.**

Resolve F1–F6 and have Chat disposition the fixes and bank replays before merge. F1–F3 are tool/harness changes and F4–F6 are documentation; this review identifies no C++ correction requiring a new game implementation build.

I independently ran the stripper's process_bytes transform **in memory on eafa654 Git blobs**. The result reproduces 14 changed files, 65 touched lines, 100 unchanged eligible files, and no touched line attributed to the four Code source/oracle commits. AnomalyCapture.Build.cs remains byte-identical. This confirms the inventory, not completion of the strip or its subsequent build checks.

After any authorized strip, Code still needs the actual diff review, both Game/Editor build checks and staged-executable A44/provenance checks required by the inventory. None ran during this read-only review. Any non-comment change widens the review scope.

AnomalyInjector.uplugin:23–26 still permits AnomalyBench in Development Game/Editor builds. The removal described at PRE-DELIVERY-CHECKLIST.md:58–66 is an explicit client packaging step, not an automatic descriptor guarantee. It remains a delivery gate: remove the module source/descriptor entry for that package and verify the resulting artifact. The separately gated null_effect/solid_swap registrations are in AnomalyInjector, so removing AnomalyBench does not remove those Development entries. The new catalogue/readme distinction is correct.

Known camera-cut/hitch coverage gaps, the absence of a producer-generated gradual ramp, legacy pairing defects under stress, and the declared unsupported/unexercised cases remain visible in the reports. This bounded review neither reran all published test suites nor replaced those missing runtime exercises with offline claims.

**Next action: Chat review REQUIRED** to accept or revise this disposition and issue Code a bounded fixes-and-replay brief. The next mandatory checkpoint is Chat's review of the corrected oracle/gate negative cases, the unchanged-bank replay results and corrected client prose **before authorizing the no-ff merge**. Current review work is complete; no probe or capture remains in flight. No repository state or continuity handoff was changed.

VERDICT: CHANGES REQUIRED
