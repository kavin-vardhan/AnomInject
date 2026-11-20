# 081-15 — completion clock implemented; qualification stopped on partial 1080p yield

2026-09-23. **NEEDS-DECISION.** Chat's cause read is confirmed. One candidate
build completed for all three targets. Both closure can-fails passed in both
orders, as did three of the four new StackOBot 1080p twins. The fourth measured
16/20 required pairs with honest capacity/predecessor refusals. No retry or
second build occurred. The remaining qualification is UNRUN; Stage 3 is held.

**Qualification-scope disclosure:** the full-pair-yield requirement for the new
StackOBot twins was **Codex's stronger predeclared criterion**. Chat's 081-14
ruling explicitly imposes full yield on **Lyra G270**; it does not explicitly
apply that same bar to StackOBot twins. The failed Stack twin has all five
onsets measured at 1.0 and no closure/late-result failure. I preserved the failed
audit and stopped instead of changing my criterion after seeing the result.
This distinction needs Chat's disposition; it is not another defect attributed
to Chat's clock specification.

**Chat review REQUIRED** for the StackOBot yield criterion and continuation.
**Chat review NOT REQUIRED** for the completed authorized diagnosis, correction,
build, captures, cause read and restoration. Next mandatory checkpoint is the
ruling requested under Questions for chat. No Code brief, merge, tag or cook.

## 1. Cause read: confirmed, with measurement boundaries

Read `081-14-chat-ruling-closure-clock-g270.md` in full before edits/build.

- All 19 old G270 late rejects are `path=colour_writer`: SI20–27 at cursor28,
  and SI77–88 at cursor89 (SI79 is not one of the rejected completions).
- Fourteen labelled rows have frozen-mask timestamps. Their exact mask-freeze
  to rejected colour-arrival latency is **617–780 ms / 19–23 engine frames**.
  This refines Chat's approximate .65–.8s; it does not contradict the mechanism.
  The two bursts have similar delays. These are not issue-to-completion samples
  for all 150 rows, so no full-run distribution is inferred from them.
- Old phase/run closure used eight additional **issued** indices or two seconds.
  Phase closure could sweep unfinished work before those colour writes arrived.
  The wall allowance was longer than these observed arrivals. Old ordering-gap
  logic also advanced its four-frame clock on issues after detecting one later
  completion. All three clock sites are corrected below.
- Both accepted Lyra720 twins had zero late results. Each lost eight window
  pairs directly to `budget_exceeded` and four to `predecessor_missing`: twelve
  total refusals, not twelve direct capacity refusals.
- Old G270 high-water 39,398,400 bytes equals three full1080p colours plus seven
  masks. Even one more full colour would stay below67,108,864. With gate14 not
  injecting capacity faults, the separate three-colour cap explains refusal
  below the byte limit. Removing that count cap is justified.

The cause is supported by the source and logs; resolution alone is not proven
to force failure on every host/content recipe. The A reading below deliberately
tests that distinction. Pilot metric: **one Chat spec defect caught by a gate**,
not an implementation escape. Old artifacts and dispositions remain unchanged.
Evidence: `cause-confirmation.json`, `cause-timing.json`, original G270 logs/source.

## 2. Source change and exact clock sites

Built source: `5649a6dddbb46fa32c4b056db92d13e8e37bb090`.

| Site in AnomalyCapture/Private | Change |
|---|---|
| AnomalyChangeStage.cpp:172 | Count distinct received colour notifications beyond an index; failures count, issues/duplicates do not. |
| AnomalyChangeStage.cpp:287 | Run terminal test: eight completions beyond immutable closure watermark or wall deadline. |
| AnomalyChangeStage.cpp:297 | Phase terminal test: eight completions beyond phase last labelled index or wall deadline. |
| AnomalyChangeStage.cpp:315 | Out-of-order test: four later completions while head incomplete, outside closing phase/run. |
| AnomalyChangeStage.cpp:446 | Run wall backstop becomes five seconds. |
| AnomalyChangeMeasurement.cpp:69 | Phase wall backstop becomes five seconds. |
| AnomalyChangeStage.cpp:157 | Remove colour-count admission cap; unchanged64MiB budget is the sole buffer admission cap. |
| AnomalySveCapturer.cpp:370 | G11 bench delay3.5s→6.5s to cross the new5s deadline; no ordinary capture-path change. |

**Precedence was explicitly declared before build:** a closing phase uses its
eight-completion/five-second rule, rather than being preempted by the ordinary
four-completion gap rule. The new `bAwaitingClosure` condition encodes this
interpretation. The eight-completion can-fail demonstrates it. Run closure stops
issue at its watermark, so ordinary run-tail timeouts use the wall arm; no newer
indices are invented to exercise a run completion-count path.

Completion membership lives in pending items and therefore follows their existing
generation/lifetime. No writer, immutable receipt/token, filter-then-freeze,
numeric contract, refusal enum, `observable`, subtraction or shader change.
`DeferredEndAt` still identifies the pending GT label sample before event end;
it is a watermark ordering check, not an issued-index timeout clock.

Additive latency diagnostics record GT issue→accepted colour completion in ms
and engine-frame increments per pair; summary sample count and nearest-rank
p50/p95/max. Missing completions use -1; late arrivals never rewrite final rows.
Summary covers all accepted notifications, not only the K=4 exported pairs.
Failed deliveries count; masks and rejected late notifications do not. These
are diagnostics, never identity/measurement predicates. Field reference updated.

Bench16 discards SI3's real stage colour notification; bench17 discards all such
notifications. Independent legacy writes still complete. The devices do not
fabricate receipts, force a refusal result or modify the writer. Timeout logs
name the actual count/wall trigger. G282 records the completion-clock gotcha.

## 3. Predictions and one build

The dated plan amendment and round journal were written before the A reading
and build. `predictions-before-build.json` records hashes; the source commit
contains those predictions. Historical predictions remain below the amendment.
All old 8issued/2s/3colour predicates were explicitly superseded before results.

One build set: StackOBot Game, StackOBot Editor, Lyra Editor, all exit0.
Game SHA-256:
`e8e2e43fa1030a338375d87d2062e4de3289e62db355751d6135a3c5e8aea90b`
(E8E2E43F),241,743,872 bytes. Built/staged/archive equality verified before each
B leg. Lyra eleven-file archive is `m55-stage2-r15-lyra-163acac8`.
Main Editor modules were archived separately. Five cooked container hashes are
unchanged. No second source build or runtime tuning.

### Preserved harness fault

The first A launch stopped before capture: the reused preflight checker expected
1280x720, while this request specified1920x1080. Its snapshot camera was the
correct origin/rotation, but viewport/pixel-coordinate scope was wrong in the
checker. Original logs/audit remain `HARNESS-INVALID`, not a failed measurement.
One harness repair validates the exact requested viewport and normalizes projected
rectangles to the unchanged720p reference. Settle0,90samples,full modal coverage,
target/component/rotation and foreground checks remain. This normalization is
explicitly declared, not a measurement-fitted calibration or a claim of original
B1 calibration at1080p. The old CaptureBench checker was not edited.

## 4. A-side StackOBot1080 reading (A699E9EE, not a gate)

`A_SOLID_1080_N_R1`: five events; **13/20 required pairs measured**, four
`budget_exceeded`, three `predecessor_missing`; foreground87/87. All five onsets
measured. Zero closure_timeout/late_results; retained0 and invariants0.
High-water **49,803,264 bytes**.

Readback latency log:90samples, min1/max2/mean1.344 engine frames, histogram
1:59/2:31 (p50=1,p95=2). This is GPU readback latency. The old candidate does not
export successful colour-writer completion timestamps; exact writer-completion
distribution is unavailable, not substituted with drain latency. No late-reject
lines exist from which to recover that timing. This host reading did **not**
reproduce Lyra's late rejects; the ruling explicitly allowed either A result.

## 5. B-side completed readings

| B leg | Measured / required | Events | Latency ms p50 / p95 / max | High-water bytes | Disposition |
|---|---:|---:|---|---:|---|
| COUNT_FAIL_N | 28/30 | 15 | 158 / 163 / 172 | 40,550,400 | PASS |
| COUNT_FAIL_S | 28/30 | 15 | 161 / 168 / 179 | 40,550,400 | PASS |
| WALL_FAIL_N | 0/30 | 15 | no completions | 27,648,000 | PASS |
| WALL_FAIL_S | 0/30 | 15 | no completions | 27,648,000 | PASS |
| NULL1080_N | 20/20 | 5 | 407 / 456 / 473 | 66,392,064 | PASS |
| NULL1080_S | 20/20 | 5 | 428 / 488 / 536 | 66,392,064 | PASS |
| SOLID1080_N | 20/20 | 5 | 472 / 525 / 534 | 66,392,064 | PASS |
| SOLID1080_S | 16/20 | 5 | 511 / 557 / 619 | 64,327,680 | FAIL: declared full yield |

All eight captures were fixture-valid, with90-snapshot preflight and full capture
foreground. All retained-byte, arithmetic, persistence, row-drop and late-record
mutation invariant counters were zero. Detailed controls, completion sample
coverage and timeout paths are in `final-readings.json` and the raw audits.

### Both can-fails fired, both orders

Count fault: exactly the intended SI3 notification is missing; closure logs show
`clock=completions`,eight later completions, before5s. Each run has15events,
28/30measured pairs: one `closure_timeout`, then one `predecessor_undelivered`.
Eighty-nine accepted completion latency samples; the missing row remains -1.

Stalled fault: zero accepted colour completions; timeout logs all use `clock=wall`,
zero later completions and elapsed time>=5s. All15events/30required rows finalize
honestly unmeasured, retained bytes0, no hang. The normal auditor's "no measured
window pair" diagnostic is preserved as expected for this deliberately stalled
case; no other measurement/identity/arithmetic assertion is suppressed.

### Healthy1080 twin observations

Natural null onsets:.001457–.002003; synthetic null:.000120–.000226. Both solid
orders:all five onsets1.0. Controls are published unsubtracted in the evidence.
The first three twins measured20/20pairs; the synthetic solid measured16/20.
Each healthy twin has90accepted completion samples and zero closure_timeout,
out_of_order_timeout or late_results. Resolution-envelope risk remains visible:
the largest high-water is **66,392,064 bytes**, near the67,108,864-byte cap.

### Synthetic solid: preserved partial-yield failure

`B_SOLID1080_S_A1`: five final/legacy events, foreground82/82, exact normalized
camera precondition, all five onsets measured at1.0. The four refused rows are:

| Saved index | Event | Window index | Reason | Completion ms |
|---:|---|---:|---|---:|
|26|solid_swap@1248|3|budget_exceeded|473|
|45|solid_swap@1272|2|budget_exceeded|512|
|46|solid_swap@1272|3|predecessor_missing|461|
|66|solid_swap@1296|3|budget_exceeded|505|

High-water64,327,680bytes, retained0. Across all90identity indices:6budget,
4predecessor_missing,22mask_payload_missing,1first_frame,57identity-valid.
The required-window subset is3budget+1predecessor refusal. Completion latency
p50/p95/max511/557/619ms, engine frames16/17/19. Sustained FPS29.992 with pacing
on is not a cost gate. No arithmetic, persistence or final-row-mutation defect.

The code has no remaining colour-count cap and gate14 injects no budget fault.
These capacity refusals arise from the real byte reservation predicate. A full
1080p colour is8,294,400bytes before any allocation padding; high-water plus
another full colour exceeds the configured cap. Exact simultaneous ownership at
each refusal is not exported, so no unsupported buffer-count reconstruction is
claimed. The missing predecessor follows refusal of its colour buffer.
Evidence:`solid1080-synthetic-cause.json`, unchanged original audit and raw files.

The original auditor's numeric/onset checks pass. The only failed assertion is
the added **Codex predeclared full-pair-yield bar** for Stack1080. I have not
silently applied a post-hoc waiver, claimed a Chat-mandated Stack floor, or
attributed this bounded-memory refusal to another implementation escape.

## 6. Unrun qualification and disposition

No retry of this valid result. No byte-budget increase, timeout change, smaller
resolution or second build. **Stage 3 HELD** because qualification is incomplete.

Remaining **67 qualification legs** (65 initially enumerated, plus two low-byte controls found in final inventory reconciliation):

- 24 Stack720measurement/finalization legs: static null/solid,delay3,hide/depth,
  occluded onset,blink,short,drop,runend,teardown both orders; moving twins natural.
- 24 Stackidentity/order/budget/lifetime requalifications: G1/2/3/4/5/6/7/10/11/12/
  13/15 both orders. Their Stage2 artifact alignment (preFrames8 puts SI8 inside
  the K4window) and per-index verbose traces were declared before any such leg.
  G8/G9 identity-only regressions remain prior coverage; identity contract unchanged.
- 2 actual `ChangeMaxBytes 1` controls, both orders: refusal must fire and retained/high-water bytes stay zero. These accepted budget controls were omitted from the initial enumerated list; final reconciliation adds them before any B reading. Both are UNRUN.
- 2 NoHold legs on B, using the qualified MainWorld/G2 reference policy.
- 12 legacy legs: both six-leg MAIN/CB comparisons against A699E9EE; MAIN strict
  declared mask/count/camera/onset/schema invariance, CB occupancy and own-tag checks.
- 3 Lyra legs: new-build720twins and300-frame G270 requalification. No Lyra capture
  ran this round; G270's three fixture attempts remain unused.

The prior A699 NoHold both-orders GREEN and accepted Lyra720twins remain exactly
as Chat accepted them; no old reading is reassigned to the new binary. Old G270
A2 remains a valid failed measurement. Its newly declared300-frame branches
are untested: all phases measured but<3events is non-blocking; three fixture
invalids permit an UNRESOLVED dropped observation; valid closure/budget refusal
is a stop. The earlier first-leg auto-drop is not invoked in this round.

Recommended next step: **Chat review REQUIRED** to decide whether the Stack1080
result should be retained as partial coverage under the unchanged64MiB budget,
then continue the remaining gates on this exact build. That would be an explicit
dated qualification amendment, with this failed audit retained. Do not infer
that it relaxes the separately mandated full-yield G270 gate. If Chat wants full
Stack yield too, the allowed correction is a new scope decision, not an automatic
retry or a reason to raise the byte cap privately.

## 7. Coverage limits — historical block preserved verbatim

- **UNEXERCISED — same-size camera cut:** accepted v1 limitation. Camera delta is a caveat, never an identity-validity rule.
- **UNEXERCISED — natural letterbox/viewport resize:** this fixture has no usable camera component for that change. G9 alters receipt geometry and tests the refusal, not a real viewport resize.
- **UNEXERCISED — dedicated projection/FOV-only producer leg:** quantised fields are exported; no claim that a dedicated optical change was forced.
- **UNEXERCISED — deliberate clean hitch:** invalid handled-ensure hitches do not count as coverage.
- **UNEXERCISED — producer-side gradual ramp:** requires asset/cook work outside v1. Accepted delay-3 end-to-end and `0,3,6,9,12` selftest belong to Stages 2 and 3 and have not run.
- **UNEXERCISED — clean actual synchronous fallback:** its attempted device hit a D3D12 ensure. G13 covers an explicitly synthetic registration gap while normal delivery continues.
- **UNEXERCISED — standalone teardown before ordinary FinishRun and separate-run stale-work transition:** code performs best-effort teardown persistence; measured gates cover run-end and within-run epoch reset. Stage 2 teardown/phase/event finalization gates remain mandatory.
- **UNEXERCISED — real owner viewport/world replacement:** G15 tests a synthetic epoch transition with real unserved arms; G7 tests actual in-flight old-generation drains. Neither is a real world replacement.
- **UNEXERCISED — JPEG/resampled/backbuffer delivery refusals as dedicated clean runtime legs:** the support checks exist, but final accepted legs use native PNG SVE delivery. Actual multi-view GPU rendering is also unexercised.
- **UNEXERCISED — the other three supported colour formats:** runtime supplied `PF_A2B10G10R10` (18), masks `PF_R8_UINT` (57).
- G8 exercises a real throwaway GT family and owner check, plus duplicate RT claim; no second GPU-rendered family or whole duplicate callback invocation is claimed.
- G11 is a nine-frame lifetime fault leg, not three-event campaign evidence. All ordinary final gate legs must meet the independent three-event rule.
- Stage 2 arithmetic, phases, event records and bench twins, and Stage 3 oracle/cost/release gates are **UNRUN**. Stage 1 sidecar rows include unlabelled indices; absence of a mask there is an explicit refusal.

### Dated amendments and Stage3carry-forwards,2026-09-23

- Stage1closed on36accepted legs (Stack30,Lyra6). Other24LyraG1–G12 legs are
  DROPPED-BY-OWNER-DECISION per081-07: host-selected spawn and no placement lever
  in the accepted Stage1candidate. This is historical scope, not current G270 drop.
- Prior24Stage2measurement+12legacy comparisons stand on their recorded binary.
  OpenLevel teardown passed both orders. Separate-run stale work and real owner
  viewport/world replacement remain unexercised. Candidate B requalification
  remains incomplete as listed above; it does not inherit source-changed gate passes.
- Delay3 end-to-end passed previously: first3adjacent differences<.1,index3>.9,
  near-null onset; retained pre-onset reference exposes accumulated change.
  Client paragraph and0,3,6,9,12verifier selftest remain due; producer ramp unexercised.
- Preserve **FPS21.5vs30 on evidence-OFF MAIN4** and explain it before any cost
  reading. A/B readings here are paced and do not clear that gate.
- Preserve high-water priors **17.5/19.4/39.4MB**. Add A1080**49.803264MB** and
  B1080maximum**66.392064MB** beside them, not in place of them.
- Stage3Stack-only pacing-off720/1080A/B/B/A must report required-pair measured/
  required yield and refusal reasons as well as completion-latency distributions,
  game-thread/worker/writer cost,queues,bytes,drops,tails. <=+1ms game thread AND
  unchanged writer throughput within uncertainty; otherwise UNRESOLVED.
- Field/client docs must state the64MiB resolution envelope: one1080p colour is
  8.2944MB before allocation padding, plus masks/predecessor/reference/pending work.
  1440p/4K are unexercised and have greater refusal pressure. This round supplies
  an actual1080partial-yield example; don't promise complete1080coverage.
- Deferred `positive_frames` counts raw deferred anomaly rows; NoHold67 does not
  mean67labelled positives. Stage3field reference and Code client-reader review
  remain due; no m55counter semantics change.
- Stage3oracle (arithmetic/transport only,peak machinery unchanged),selftests,
  cost qualification,full client docs and independent Code review remain UNRUN.
  Preserve all historical limits verbatim with these dated amendments.

## 8. Preservation, state and ownership

Ten new physical launches:nine captures,one pre-capture harness stop. One A-side
reading and eight B gates; seven B passes,one valid predeclared-yield failure.
One harness repair,no gate retry,no Lyra launch. One three-target build set.
Source/prediction commit5649a6d; documentation disposition commit/full feature
head recorded in `postflight.json`. Feature commits pushed; master/m51 untouched.

Main checkout restored to m51
`53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`. Main Game/Editor retain the new
candidate binaries despite m51source; **E8E2E43F is not yet qualified for release**.
Lyra restored to detached`caa68c6799db023f7a24c0d6cfc56124ce27b542` and original
nine modules; the two candidate-only Bench files removed by exact verified paths.
Prior archives,all cooked containers,CaptureBench,untracked owner handoffs and
earlier banks/reports/evidence remain hash-verified. All prior CURRENT bytes are
preserved,including unrelated Blender/video history. No owned process is in flight.

Boundary proof:`lyra-restored.json`,`restoration.json`,`postflight.json`; if these
are absent or fail,the boundary claim is invalid. `081-15-frozen/manifest.json`
names source/feature commits and frozen source/docs. Bank/evidence manifests
retain all attempts,including the failed yield and first harness fault.

Read in order:this report FULL;081-14ruling FULL;frozen081-15journal/plan amendment;
cause confirmation/timings;A reading;final-ledger/final-readings;all eight audits
and raw captures;partial-yield cause;binary/build/frozen/bank/postflight manifests.
Same coherent workstream,no fresh-session recommendation. Codex implements,
Chat orchestrates,Code independently reviews afterStage3. Next mandatory Chat
checkpoint is the Stack1080qualification disposition; no in-flight work to inherit.

## Questions for chat

1. May the synthetic Stack1080solid result stand as **partial coverage** under
   the unchanged64MiB budget (all5onsets1.0,16/20pairs,3budget+1predecessor refusal),
   with an explicit dated amendment to Codex's stronger predeclared full-yield
   criterion? The ruling explicitly required full yield for Lyra G270, not these
   Stacktwins. Preserve the original failed audit; no result retry is proposed.
2. If so, release the remaining 67 qualification legs on **the same
   E8E2E43F build**, retaining the300-frame G270 branches and its full-yield bar.
   If full Stackyield is required too, please rule on the permitted correction;
   the single build allowance has been used and the64MiB cap remains unchanged.

VERDICT: NEEDS-DECISION
