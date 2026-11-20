# 081-14 — NoHold qualified; scoped Lyra G270 observation unresolved

2026-09-23. **NEEDS-DECISION.** NoHold passes in both orders, and both Lyra
twins pass. The fixture-valid G270 attempt produced only two events and no
measured evidence pair. Scoped R5 is incomplete, so the conditional release
to Stage 3 has not been met.

**Chat review REQUIRED** for the G270 failure disposition and further work.
**Chat review NOT REQUIRED** for the completed, authorized requalification,
captures, bounded cause read, documentation and restoration. The next mandatory
checkpoint is Chat's ruling on the questions below. No Code brief was dispatched.

Authority: `081-13-chat-ruling-nohold-requalify-g2.md`, read in full, including
the pre-dispatch tick-order-only clarification; inherited 081-03 and 081-07.
No implementation change or build. StackOBot used A699E9EE; Lyra used the
qualified eleven-file 6ef55ade archive. All earlier accepted 24 measurement
legs and both six-leg legacy comparisons remain accepted.

## 1. NoHold natural: dated post-hoc precondition amendment

**2026-09-23 amendment: exactly ONE in pilot metrics, affecting two artifacts.**
Requalified the same 081-13 A2/A3 captures against independent m52
`M52E_M52E_G2_CANFAIL/session_20260920-214247`, joined on saved `session_index`.
Both match **90/90 rows at 0 cm / 0 degrees**, within the unchanged 1 cm / 0.1-degree
tolerance. No new natural launch; the natural allowance remains exhausted.

Original G3 audits and failed-pose-precondition classifications remain untouched
in 081-13 (`INVALID-FIXTURE` in the original ledger). The new ledger embeds those
original dispositions alongside `PASS-REQUALIFIED`. The original CB Cube A1
remains `FAILED-PRECONDITION(recipe)`. No claim that the old precondition passed.

The prior diagnostics now become the qualified gate readings for each artifact:
three applied fires; `held:false` throughout deferred telemetry; three final
events with `phase_count:0` and `no_labelled_frames`; zero pairs and zero legacy
injected frames; three legacy events + zero vetoes = three finals. Applied-fire
identities are engine frames 20/58/96. Two events end by removal/timeout, the last
by run end; NoHold bias suppression is logged. Chat accepted the earlier audit
correction to count actual applied-fire lines instead of M23 first-arm lines.

Evidence: `081-14-evidence/natural-requalification.json` and
`pilot-metrics-delta.json`, including reference hashes and the old dispositions.

## 2. NoHold synthetic: PASS on attempt 1

The local M52E inventory agrees with Chat's bank lookup: G2 is NoHold natural;
G10 is HOLD synthetic; no NoHold synthetic twin exists. No contradictory bank
was found (`m52-bank-lookup.json`).

Declared before launch: use only the tick-order exception, reference G2 on engine
`frame_index`, every counterpart within 1 cm / 0.1 degrees and at least 80/90
matched. Map, target, NoHold, config, seed and flags stayed the same; only tick
order and the unique output label changed. No placement, lock, AA/timeout
override or build was introduced.

`M55S2R14_NOHOLD_S_A1` passed **90/90 counterpart rows at 0 cm / 0 degrees** and
53/53 foreground samples. Three actual applied fires with matching identities,
all `held:false`, three zero-phase `no_labelled_frames` finals, zero pairs,
three legacy events, zero vetoes and zero legacy injected frames. Synthetic
used one of three attempts. High-water 7,372,800 bytes; retained bytes and
arithmetic/persistence/late-mutation/drop invariant counters were zero.

**Counter caveat carried to Stage 3:** each NoHold capture has 67 deferred legacy
telemetry rows and `positive_frames:67`, while injected frames are zero. Those
rows have `target_pixels:-1`, `observable:null`, `held:false`. Raw anomaly rows
and this counter do not prove labelled positive windows. The field reference
must explain deferred-onset counting, and Code must review client-reader
assumptions. No m55 counter code change is requested.

## 3. Scoped R5 Lyra

Only natural null/solid twins and the G270 observation were attempted. Source
cb14f31 and all eleven archived modules were verified before launches. Host AA
and exposure defaults were retained. The sequence was world-ready input lock,
45-second settle, one compiled L2 placement, independent fresh L1 within
150 cm / 10 degrees, then capture. The full log audit checks the
placement-to-capture interval for respawn/possession, including buffered lines.
Foreground is required throughout capture. Owned processes were joined after
exit. Original Lyra source and nine modules have been restored.

### Attempt ledger

| Leg | Attempt | Fixture | Measurement / disposition |
|---|---:|---|---|
| null_effect | 1 | INVALID: post-capture L2 audit | Captured, preserved, uncredited |
| null_effect | 2 | INVALID: pre-capture L2 audit | No capture |
| null_effect | 3 | PASS | PASS: 5 events, 8/20 pairs measured |
| solid_swap | 1 | PASS | PASS: 5 events, 8/20 pairs measured |
| G270 stuck_low_mip | 1 | INVALID: L1 | No capture |
| G270 stuck_low_mip | 2 | PASS | FAIL: 2 events, 0/8 pairs measured |

Null A1 passed L1 at 0.115287 cm / 0 degrees and foreground 82/82, but the
post-capture audit found death/respawn messages in the protected interval.
Null A2 passed L1 at 0.009000 cm / 0.082568 degrees, but pre-capture L2 found
restart/avatar messages after placement. Both remain invalid. Null A3 passed,
so the first-leg three-invalid auto-drop condition **did not occur**.

G270 A1 failed L1 at 1,057.457297 cm / 0.082568 degrees after a respawn following
placement. The runner correctly stopped before capture. G270 A2 passed L1 at
0.115287 cm / 0 degrees, L2 before and after capture, and foreground 138/138.
It is a valid measurement failure, not a fixture retry opportunity. No A3 ran.

### Qualified twins

| Gate | Onset gt8/n | Onset control gt8/n | Foreground |
|---|---|---|---:|
| null_effect A3 | .002807–.009017, all 5 measured | .012235–.080889 | 86/86 |
| solid_swap A1 | 1.0, all 5 measured | .003823–.050936 | 83/83 |

Controls are reported without subtraction. Each twin has eight `budget_exceeded`
and four `predecessor_missing` window refusals; 71 frozen mask CRCs verified;
high-water 19,353,600 bytes, retained bytes zero and invariant counters zero.
This establishes separation of the measured onsets, not full window coverage
or a saved-colour oracle. Full readings: `lyra-twin-readings.json`.

### G270 A2: bounded cause read

Recipe: 1920x1080, 150 frames, seed 4242, Config 2/4/8/4/0, natural order,
auto pool restricted to stuck_low_mip, MaxCoAffected 8. No content threshold was
predicted; the inherited ordinary-event floor and measured-observation gate
remain in force. The unchanged auditor reports:

- Fewer than three final events.
- Fewer than three legacy events.
- No measured window pair.

The two applied events are `stuck_low_mip@2822` on Cube4 and `stuck_low_mip@2903`
on Cube3. Each has seven held/labelled frames (saved indices 21–27 and 82–88),
one indeterminate phase and four required pairs. **All eight pairs are
`closure_timeout`; there is no numeric change observation.** Values of -1 are
unmeasured sentinels, not measured zero or NO-TRACE. Fourteen frozen mask CRCs
verified. Legacy and sidecar both have two events, with zero vetoes.

**Event yield:** the log and summary record eight `not_restored` refusals and
two `not_streamable` refusals. Both applied holds used three paint textures.
The first restoration was verified after 41 engine frames. At FinishRun the
summary still had three textures awaiting restoration and a maximum of 83
restore frames; the main-log tail later verifies all three at 84 frames.
The summary was not rewritten. These refusals explain lost firing opportunities;
they do not make the two-event run satisfy the three-event gate.

**Evidence refusal:** all 150 identity pairs were refused: first_frame 1,
predecessor_missing 7, predecessor_undelivered 2, mask_payload_missing 34,
budget_exceeded 87, closure_timeout 19. The summary reports 19 late results,
23 multi-ready colour drains, zero late-record mutations and zero arithmetic,
persistence or row-drop invariant counters. High-water is **39,398,400 bytes**,
below the 67,108,864-byte setting; retained bytes are zero.

The source gives the relevant mechanisms: `AnomalyChangeMeasurement.cpp:66`
closes a phase with a two-second deadline; `AnomalyChangeStage.cpp:256–279`
also makes eight subsequent issued capture indices terminal and refuses pending
work with `closure_timeout`. `ReserveLocked` at line 155 checks both bytes and
a **three-colour-buffer cap**, so a budget refusal need not hit the byte ceiling.
These mechanisms are consistent with the observed refusal pattern. The artifacts
do not identify which completion was missing at each timeout or which deadline
won. They do not establish that increasing the byte budget would fix it. No
timeout, capacity, recipe or source change was made to seek a better result.

Evidence: `g3-cause-read.json`, the full raw main log, `run_summary.json`,
`change_evidence.jsonl`, `annotation.json`, `labels.jsonl`, and the unchanged
`LYRA_G3_N_A2/measurement-audit.json`. Sustained FPS was 29.4936 with pacing on;
this is not a Stage 3 cost gate and does not clear the earlier MAIN4 concern.

## 4. Accounting and disposition

Seven new physical launches: one NoHold synthetic and six Lyra attempts.
Five captures started; two launches stopped before capture. Three attempts were
fixture-invalid, three new captures passed measurement, and one fixture-valid
capture failed measurement. The two natural requalifications are old artifacts,
not new launches. Exactly one post-hoc precondition amendment is counted.

Natural has zero attempts remaining. Synthetic has two unused. G270 used two
attempts, but its unused third slot does not authorize retrying a valid failed
result. No harness fault repair, build or implementation change occurred.

NoHold both orders and both Lyra twins are qualified. R5 G270 is unresolved and
has not been auto-dropped. **Stage 3 HELD; Chat review REQUIRED** to dispose of
this valid failed observation. The recommended next step is a bounded diagnosis
of closure latency/completion readiness and a separately declared qualification
recipe, if Chat retains this gate. No event-floor waiver or widened auto-drop
rule has been applied privately.

## 5. Coverage limits — historical block preserved verbatim

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

### Dated amendments carried forward, 2026-09-23

- Stage 1 closed on 36 accepted legs: StackOBot 30, Lyra 6. The other 24 Lyra
  G1–G12 legs are **DROPPED-BY-OWNER-DECISION** per 081-07, because the host
  chooses the spawn per launch and the accepted Stage 1 candidate had no
  placement lever. Their history remains; this is not a drop of current R5.
- The prior 24 Stage 2 measurement legs and 12 legacy comparison legs remain
  accepted. Actual OpenLevel teardown has passed in both orders. Separate-run
  stale-work transition and real owner viewport/world replacement remain
  unexercised. This does not turn the synthetic G15 case into real replacement.
- Delay=3 end-to-end passed: first three adjacent differences below .1, index 3
  above .9, near-null onset. This is the accepted gradual-arrival counterexample;
  the retained pre-onset reference exposes accumulated change. The Stage 3 client
  paragraph and 0,3,6,9,12 verifier selftest remain due; producer ramp unexercised.
- NoHold is now qualified in both orders; current Lyra twins pass. G270 remains
  a valid failed measurement with no numeric evidence. No saved-colour oracle
  or complete pair coverage is claimed.
- Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4** into Stage 3 and explain it
  before reading any cost gate. Preserve high-water priors **17.5 / 19.4 MB**;
  add this round's G270 observation **39.3984 MB**, without rewriting the priors.
- Stage 3 oracle, selftests, StackOBot-only pacing-off 720p/1080p A/B/B/A cost
  gates, complete field/client docs and independent Code review remain UNRUN.
  Cost requires game-thread increase <=1 ms AND writer throughput unchanged
  within uncertainty; otherwise UNRESOLVED. Peak machinery stays untouched.
- Carry the deferred-row `positive_frames` caveat into the field reference and
  Code review. No m55 counter semantics change was made.

## 6. Boundary, evidence and ownership

Source remains cb14f3109d56d9b6b25ab4ccb72472b5c0f2eae8; the feature commit for
this round contains documentation only and is recorded in `postflight.json`.
Feature branch changes are committed and pushed; no merge or tag. Main checkout
is restored to m51 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67. Candidate main
Editor/Game binaries remain in place despite the m51 source checkout, as at entry.
Game built/staged/archive SHA-256 remains
`a699e9ee46d43a860912977cc8ed721cf8233958398f9cab9b0f2b43bf36a46d`.

Lyra is restored to detached caa68c6799db023f7a24c0d6cfc56124ce27b542 and its
original nine modules; the two candidate-only Bench files were removed by exact
verified paths. Master, m51 refs, CaptureBench, archives, containers and existing
untracked files are preserved. CURRENT retains every prior byte, including
unrelated Blender/video history. No game/build/capture remains in flight.
`restoration.json` and `postflight.json` verify this boundary; their absence or
failure would invalidate this boundary claim.

Read in order: this report in full; 081-13 ruling in full; frozen round journal;
`final-ledger.json`; natural requalification and synthetic audits; Lyra results
and G270 cause read; raw captures/logs; manifests and postflight. The frozen
source/docs bundle is `081-14-frozen`, with source and feature commits recorded.
Original 081-11/12/13 banks and prior evidence/report hashes are checked again.

Codex remains implementer, Chat orchestrator, Code independent reviewer after
Stage 3. This remains the same workstream; no session switch is recommended.
The next mandatory checkpoint is Chat's G270 ruling. Stage 3 can continue only
after R5 is resolved under the governing ruling and the report is GREEN.

## Questions for chat

1. Rule on the fixture-valid G270 failure: two applied events and zero measured
   pairs, all eight required pairs `closure_timeout`. May Codex diagnose and
   correct completion/closure behavior if needed, then requalify under a newly
   declared G270 recipe? The existing three-event floor and refusal semantics
   remain intact unless Chat explicitly changes the qualification scope.
2. Confirm the disposition if Lyra cannot supply this observation under its
   scoped recipe. The 081-07 automatic drop applied only to three invalid first
   legs; null A3 passed, so it does not authorize dropping G270 here.

VERDICT: NEEDS-DECISION
