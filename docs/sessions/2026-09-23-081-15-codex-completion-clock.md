# 081-15 — completion clock correction and requalification

2026-09-23. Authority: 081-14-chat-ruling-closure-clock-g270.md, read in full.
Cause confirmed from full source/logs. Precise mask-to-colour times .617-.780s,
19-23 engine frames for 14 labelled samples; all19late rejects colour_writer in
the declared ranges. Both Lyra720 twins zero late. Eight direct budget + four
dependent predecessor refusals per twin. High-water plus one complete buffer
cannot hit64MiB: the separate3-colour cap caused capacity refusal. Resolution is
the supported explanation; the A-side reading checks the home fixture directly.
One Chat spec defect caught by gate; no implementation escape. No old evidence
or disposition rewritten. Chat accepted NoHold both orders and Lyra720 twins.

## Dated amendment BEFORE build and new captures

This prospectively supersedes every old accepted prediction encoding 8 issued
indices /2seconds /3colours, including D4/D7, G-LATE/G-EPOCH/G15, budget, phase,
short/drop/run-end/teardown gates. Historical text and readings remain intact.

- Phase closure: eight distinct received colour completions (success or failure)
  for indices beyond that phase's last labelled index OR five seconds wall.
- Run closure: same count beyond immutable closure watermark OR five seconds.
  Issue stops at run closure, so a normal finished run has no newer indices;
  its unresolved tail necessarily relies on the wall backstop. Do not invent
  completions or admit beyond the watermark to make a count test pass.
- Out-of-order: four distinct later completions while the head is incomplete,
  never advances merely on issue. A closing phase uses its closure rule;
  the four-completion gap rule applies outside phase/run closure. This precedence
  gives the requested eight-completion phase rule its full bound instead of
  preempting it after four. No receipt/identity or writer behavior changes.
- Admission:64MiB compiled byte budget ONLY, no colour count limit. Reserve
  before retaining; writer never waits. All refusal names/numerics unchanged.
- Additive issue-to-colour-completion diagnostics: milliseconds and engine frame
  span per accepted completion/pair, plus run sample count and p50/p95/max.
  Missing/late completions are unmeasured for row latency; no final-row mutation.
  Report coverage/late exclusions. Latency is diagnostic, never a validity rule.
- Existing G11 delayed-real-readback can-fail must cross the new5second wall
  bound: bench-only delay becomes6.5seconds, frozen-row equality still required.

## A-side reading, before the one build

Exactly one StackOBot CB_GateLevel solid_swap NATURAL at1920x1080, A699E9EE,
90frames, seed777, Config2/4/16/4/0, default64MiB, ChangeEvidenceCases1/Gate14,
SVE1/Delivery0/Pace1/Fps30/AA0/AEoff. Qualified input-lock/placement/B1 runner.
Predict capacity refusals and possible late rejects at1080; this is a reading,
not a gate, and either result stands. Publish required-pair yield, reasons,
high-water and available latency evidence; A has no new completion diagnostics.

## One B build and declared regression set

Build one source candidate for StackOBot Game/Editor and LyraEditor; no cook.
Freeze source, hashes and modules; preserve A699 and original Lyra module set.
Fixture allowance3 per gate, actual first fixture-valid result stands. No retry
of a valid failed measurement. Stop on failed gate/build or ~4h stage bound.

1. StackOBot Stage1 closure/order/budget/lifetime sites: G1(two-ready), G2(skip),
   G3(writer failure), G4(readback failure), G5(mask-write failure), G6(coalescing),
   G7(epoch), G10(budget), G11(late), G12(stale predecessor), G13(registration gap),
   G15(unserved-arm reset), both orders. Gate14 healthy control is covered by all
   measurement legs. G8/G9 identity-only faults remain accepted; no identity or
   receipt/geometry contract change. Lyra Stage1 stays scoped out by081-07.
2. Every accepted Stack Stage2 measurement/finalization recipe, both original
   orders where accepted: static null/solid, delay3, hide, hide_depth, occluded
   onset, blinking phases/cap, short event, target drop, run-end, actual teardown;
   moving null/solid natural. NoHold natural/synthetic use qualified MainWorld
   recipe and081-13 same-recipe reference/exception, not obsolete Cube device.
   Original per-leg event floors and approved lifetime exceptions unchanged.
3. Both six-leg legacy comparisons vs A699E9EE: MAIN strict declared non-evidence
   count/camera/onset/schema/mask-byte checks; CB same occupied pixels plus every
   nonzero byte equals that run's own tag. No changing legacy observable/schema.
4. New Stack1080 null/solid, BOTH orders, same90frame twin recipe as A. Predict
   all required onset/window pairs measured with invariants0, null below solid;
   report control unsubtracted. Preserve720p and1080p yields/reasons/latency.
5. Two explicit can-fails, both orders, null short recipe Config2/4/2/4/0:
   gate16 discards the real stage colour-completion notification for SI3 only;
   ordinary legacy writer remains untouched. First phase should close after
   eight real later completions, log clock=count and count>=8, <5s wall.
   gate17 discards all stage colour-completion notifications: no colour
   completions; wall-only closure after5s, no hang, final records, retained0.
   These are real missing notifications at stage ingress, not fabricated
   receipts or forced refusal results. Enum/numerics unchanged. Logs establish
   trigger path; measurement auditor allows the deliberately refused first/all
   phases, still checks exact required rows and accounting. No positive claim.
6. Re-run accepted Lyra720 natural twins because admission/closure predicates
   changed; same L2/L1/foreground, allowance3, first valid result. Then G270 A2
   exact CLI except300frames, fixture allowance3, first valid is final reading.
   Pass>=3events and EVERY labelled phase's required pairs measured, invariants0,
   report readings/control, no threshold. Every phase measured but<3events:
   OBSERVED-BELOW-EVENT-FLOOR, non-blocking. Three fixture-invalid attempts:
   dropped/UNRESOLVED observation, non-blocking. Any refused required pair in a
   valid run remains visible; closure_timeout/budget_exceeded =>NEEDS-DECISION.

## Boundaries and carry-forwards

No writer change, identity change, enum/schema/observable change, subtraction,
cook, merge or tag. Restore mainm51 at round boundary and Lyra original source/
nine modules; disclose candidate main binaries. Preserve all banks/manifests,
owner handoffs and entire CURRENT history. Feature commit/push authorized.
Report081-15; GREEN auto-releasesStage3 per081-03. Chat review NOT REQUIRED for
this authorized work. Chat review REQUIRED on contradiction/material new finding,
failed gate or report checkpoint; no Code brief. Stage3 carries all prior limits
verbatim+dated amendments, FPS21.5vs30 MAIN4, delay3 paragraph/selftest,
positive_frames deferred-row caveat, priors17.5/19.4/39.4MB plus new readings.
Added Stage3: required-pair yields/reasons and latency distributions per cost leg,
resolution envelope64MiB vs1080p colour8.2944MB,1440p/4K unexercised and more
budget pressure; gotcha: an issue count is not a completion clock.

### Before A-side capture: harness correction1

First launch stopped beforecapture: inherited B1 checker hard-coded720p.
Original retained HARNESS-INVALID; no capture reading. New checker validates
exact declared viewport and compares normalized rectangles in unchanged720p
reference coordinates; full90samples/settle0/coverage1/rotation0/target checks
remain. This is an explicit resolution normalization, not a new calibration
value fitted to measurement. Rerun is the one authorized A-side reading.

### A-side reading before build

A_SOLID_1080_N_R1 completed on A699E9EE: B1 normalized-reference PASS90/90,
foreground87/87,5events,13/20required pairs measured,4budget_exceeded and
3predecessor_missing,0late results,0closure_timeout. High-water49,803,264bytes,
retained0/invariants0. Readback log latency90samples: min1/max2/mean1.344frames,
histogram1:59/2:31 (p50=1,p95=2). This is GPU readback latency, not writer completion.
A has no success-completion timestamps; exact issue-to-writer distribution is
unavailable, not inferred from drain time. Unlike Lyra1080, this home fixture did
not reproduce late rejects; report the resolution/host interaction without claiming
that resolution alone forces failure. The ruling explicitly accepts either A reading.

### Stage1 fault requalification artifact alignment, before B-side legs

Stage2 sidecar exports only K=4 labelled windows, while Stage1 exported every
identity row. Requalification uses Config2/8/16/4/0 for the Stage1 fault recipes:
first phase begins atSI7, placing fixed faultSI8 inside the required window.
Enable existing LogAnomalyCapture Verbose PAIR tracing to reconcile every issued
index, including unlabelled/fault rows; no code/clock change. G11 remains9frames
and its prior lifetime-only event-floor exception. Receipt-based predicates and
real injected fault proof stay the same; Stage1's old all-rows sidecar audit is
not applied to the intentionally narrower Stage2 artifact. Measurement audit
plus per-index identity traces and original fault-specific assertions are used.
No new budget or closure predicate is introduced after build.

## Results/disposition,2026-09-23

One build set passed all3targets; source5649a6d; GameE8E2E43F fullhash in
081-15-evidence/binary-v1.json. Count/wall missing-completion can-fails PASS both
orders. Count:15events,28/30pairs,SI3closure_timeout after8real later completions
plus predecessor refusal; wall:15events,0/30pairs,zero completions,5sbackstop.
1080null N/S andsolid N PASS20/20pairs each. Solid S valid fixture but16/20:
SI26/45/66budget_exceeded,SI46predecessor_missing. All5onsets measured1.0,
0closure/outoforder/late,0invariants,retained0; highwater64,327,680bytes.
MaxBhighwater66,392,064bytes. Full latency/control data in final-readings.json.

Qualification-scope disclosure: full-pair yield on Stacktwins was Codex's stronger
predeclared predicate; Chat explicitly imposed it on LyraG270, not Stacktwins.
Original failed audit retained; no post-hoc reclassification, result retry,
budget change or second build. Chat review REQUIRED for this scope/result
disposition. Recommend explicit partial-coverage acceptance for Stackthen remaining
gates on samebuild; do not infer relaxedG270bar. Stage3HELD;65declaredlegsUNRUN:
24Stack720measurement,24identity/order/budget/lifetime,2NoHold,12legacy,3Lyra.
NoLyra capture thisround;3G270fixtureattempts unused. PriorA699acceptedgates stand.

10launches,9captures:1pre-capture harness fault/fix,1Areading,8B(7pass/1fail).
Lyraoriginalsource9modules restored; mainm51 restoredafterdocscommit/push with
newcandidateGame/Editorleftasdisclosed. Oldarchives/containers/banks preserved;
postflightverifiesboundary. Chat review NOT REQUIRED for completedauthorized
work/safe restoration; nextmandatorycheckpointChatStack1080yielddisposition.
Report081-15NEEDS-DECISION; not release readiness. Addedcarryforwards include
required-pairyield,latencydistributions,resolutionenvelope,priors17.5/19.4/39.4MB
plusA49.803264/B66.392064MB. Allolderlimits/FPS/delay3/countercaveats retained.

### Final inventory reconciliation (no additional leg)

The initial65remaining count omitted accepted ChangeMaxBytes1 controls in both
orders. Add those2 UNRUN byte-admission controls: total67remaining. Their old
refusal/highwater0 predicates remain. No pass or completed result changed.
