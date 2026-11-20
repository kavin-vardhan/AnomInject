# 081-11 — guard revalidation and bounded Stage 2 continuation

2026-09-22. Authority:081-10-chat-ruling-fix-guard-and-revalidate.md, with081-08/09
unchanged requirements. Codex implements; Chat orchestrates; Code reviews after Stage3.

## Disposition

**Guard correction and final-binary preflight qualification PASS:5/5. Stage2 remains
NEEDS-DECISION.** Continued in the same round: **22 final-build measurement legs accepted**,
including actual teardown in both orders. Stopped at the first executed NoHold gate:
its Cube target had **zero candidate textures**, so all five injection attempts returned
not applied. Zero events exist; `no_labelled_frames` was not exercised. This is my unsuitable
recipe inherited from the unrun081-08 helper, not evidence of a missing m55 final record.

The capture passed B1 and foreground, so this is a failed post-capture gate precondition,
not a pre-sampling HARNESS-INVALID covered by the guard ruling. No retry or replacement of
that result; two nominal natural attempts remain unspent and held for disposition.
NoHold synthetic, moving twins and six-leg legacy comparison remain UNRUN. R5 remains last.

**Chat review REQUIRED** for this failed-precondition disposition, corrected NoHold recipe
and remaining R2–R4 continuation. **Chat review NOT REQUIRED** for completed authorized
correction/build/qualification, same-round measurements or boundary restoration. Next mandatory
checkpoint is Chat's ruling on this report. Stage3 remains held; no release/merge conclusion.
No new Code brief or fresh-session recommendation.

## Source, builds and archives

| Revision | Source | Game SHA256 | Role |
|---|---|---|---|
| Guard correction | `9a28f0b7dc641c0da89950549aed34210f1dc2da` | `d0867240c57d9309c5f332d8557af6f2ea8dc6d6604a0dfdfad698e45b11d60c` | 5/5 qualified; four static twins accepted |
| Remaining fixture support | `cb14f3109d56d9b6b25ab4ccb72472b5c0f2eae8` | `a699e9ee46d43a860912977cc8ed721cf8233958398f9cab9b0f2b43bf36a46d` | final qualified candidate;22 accepted gates |

Both revisions built StackOBot, StackOBotEditor and LyraEditor Development successfully:
six successful target builds, two source/build revisions, no cook. D0867240 and A699E9EE
archives are retained alongside refused B283E34C, pre-fix2C21EDE5, AEBD09EA and0844220E.
Final Game size241,736,192; built/staged/archive hashes agree. Final archive:
`D:\IntrusiveAnomalies\_binary_baselines\StackOBot.exe.m55-stage2-A699E9EE`. Final eleven-file Lyra archive:`D:\IntrusiveAnomalies\_binary_baselines\m55-stage2-fixture-lyra-6ef55ade`.
Full identities/containers/A44 scans:binary-v1.json and binary-v2.json. Positive fixture and
measurement tokens present, invented token absent; five cooked containers unchanged.

Against081-10 feature8600e4e only two implementation files changed: AnomalyBenchModule.cpp
and AnomalyControlServerSubsystem.cpp. Core measurement arithmetic, label serialization,
observable rules, shaders, materials/assets on disk, auto-pools and host source unchanged.
The module keeps non-Shipping Game/Editor eligibility; Client/Server excluded.

F1 removes GetPawn from readiness; the player controller alone suffices for input counters.
F2 resolves a pawn/spectator view target first, otherwise GetPawnOrSpectator, otherwise
`no_view_owner`. It applies the measured camera-to-owner offset once, with no replacement
pawn/camera or persistent pin. Every fixture command logs controller/pawn/spectator/viewtarget,
capture-active/map/flag before decisions; refusals identify one condition. Exact runtime
readiness proves `pawn=null`, spectator and viewtarget both SpectatorPawn. The existing host
`No Spawnpad found!` warning remains recorded and was not chased.

Revision2 adds default-off typed SceneFixture commands for the already-authorized cooked-mesh
occluder and capture-only yaw motion. A narrowly gated `benchDelayFrames=3` argument feeds the
existing solid_swap delay path only in the explicit named-map non-Shipping fixture. No generic
remote exec. Occluder construction/trace and delay path exercised; motion only built, UNRUN.

## Guard validation: executed samples, strict unchanged gates

Predeclared ON1 OFF1 ON2 OFF2 ON3 OFF3 ON4 ON5 on each revision, no captures/injections in these
16 launches.90 snapshots each. Every ON passed B1, settle sample0, modal rotation0/0/0,
100% coverage, exact camera(-1500,0,260), one lock/one placement/one explicit unlock restoring
entry flags0/0. Every OFF also passed raw B1 and applied no fixture commands. No readiness
refusal, protocol failure or handled-ensure; no harness correction retry needed.
D0867240 had90/90 foreground throughout all eight observations. Final A699E9EE:

| Observation | Settle sample | Coverage | Foreground | Result |
|---|---:|---:|---:|---|
| F12V2_ON1 | 0 | 100% | 90/90 | PASS |
| F12V2_OFF1 | 0 | 100% | 90/90 | PASS |
| F12V2_ON2 | 0 | 100% | 90/90 | PASS |
| F12V2_OFF2 | 0 | 100% | 90/90 | PASS |
| F12V2_ON3 | 0 | 100% | 89/90 | PASS |
| F12V2_OFF3 | 0 | 100% | 90/90 | PASS |
| F12V2_ON4 | 0 | 100% | 90/90 | PASS |
| F12V2_ON5 | 0 | 100% | 90/90 | PASS |

ON3's89/90 is permitted for this no-capture preflight and did not move its pose. This exemption
was not applied to measurement legs. The rule clarification is in the journal and plan fixture
amendment: pre-sampling guard/protocol/ensure faults are HARNESS-INVALID, bounded by two
fix-rebuild iterations; executed pose/B1 failures count against5/5. Second build added required
R2/R3 support rather than repairing a failed validation; conservatively recorded as revision2.
Both binaries requalified independently before measurements. Old DELAY_N invalids stay uncredited;
old accepted twins stay accepted. Final-build allowances were three attempts per leg.

## Final-build measurement evidence

Fresh90-snapshot B1 precedes every capture: input locked before5s settle, one-shot placement,
700ms wait, independent B1, then capture_start. Input held through run. Foreground PID sampled
at roughly50ms from capture start through summary/writer-tail completion; every recorded sample
must belong to the owned PID. Original captured-label B1 also required for static labelled
recipes. NoHold uses fresh pre-capture B1 plus exact captured camera checks because it is meant
to have no labelled bbox; no private geometry/offscreen exception was restored.

| Accepted leg | Sidecar / legacy events | Measured / required pairs | Foreground samples |
|---|---:|---:|---:|
| V2_NULL_N_A1 | 5 / 5 | 15 / 20 | 86/86 |
| V2_NULL_S_A1 | 5 / 5 | 16 / 20 | 87/87 |
| V2_SOLID_N_A1 | 5 / 5 | 17 / 20 | 82/82 |
| V2_SOLID_S_A1 | 5 / 5 | 15 / 20 | 87/87 |
| V2_DELAY_N_A1 | 5 / 5 | 16 / 20 | 83/83 |
| V2_DELAY_S_A1 | 5 / 5 | 20 / 20 | 87/87 |
| V2_HIDE_N_A1 | 5 / 5 | 20 / 20 | 82/82 |
| V2_HIDE_S_A1 | 5 / 5 | 17 / 20 | 86/86 |
| V2_HIDE_DEPTH_N_A1 | 5 / 5 | 16 / 20 | 87/87 |
| V2_HIDE_DEPTH_S_A1 | 5 / 5 | 14 / 20 | 86/86 |
| V2_EMPTY_N_A1 | 5 / 1 | 0 / 20 | 87/87 |
| V2_EMPTY_S_A1 | 5 / 1 | 0 / 20 | 86/86 |
| V2_BLINK_N_A1 | 4 / 4 | 147 / 173 | 220/220 |
| V2_BLINK_S_A1 | 4 / 4 | 147 / 176 | 226/226 |
| V2_SHORT_N_A1 | 15 / 15 | 25 / 30 | 107/107 |
| V2_SHORT_S_A1 | 15 / 15 | 25 / 30 | 107/107 |
| V2_DROP_N_A1 | 1 / 1 | 2 / 4 | 59/59 |
| V2_DROP_S_A1 | 1 / 1 | 4 / 4 | 57/57 |
| V2_RUNEND_N_A2 | 1 / 1 | 2 / 4 | 42/42 |
| V2_RUNEND_S_A1 | 1 / 1 | 4 / 4 | 42/42 |
| V2_TEARDOWN_N_A1 | 1 / 1 | 2 / 4 | 46/46 |
| V2_TEARDOWN_S_A2 | 1 / 1 | 4 / 4 | 47/47 |

All accepted gates passed receipt joins, denominator/histogram conservation, ordered unique
pairs, phase state/final records, K4 window limits, eight-detail cap, normalized means,
reference-onset equality and saved-mask CRC comparisons.1,233 mask CRCs verified; byte high-water
at most17,510,400; zero bytes retained at closure; denominator/histogram/ref-onset mismatches,
persistence failures, late mutations and dropped sidecar rows all zero. Refused window pairs
remain explicit; this is not100% pair coverage or an independent saved-colour arithmetic oracle.

- Static null onsets:gt8/n0.000060–0.000494; solid onsets1.0 in both orders. All20 static
  onsets measured. Controls retained as separate readings, never subtracted.
- Delay3 both orders: measured first-three-window samples below0.1 change fraction; measured index3
  transitions above0.9; all onset samples remained near null. Exact per-pair readings retained.
- Normal/depth-silencing hides: both orders passed. Depth case had drawn==count and observable
  true on71 measured labelled rows per order; no change to legacy observable assignment.
- Occluded onset: duplicate of the actually loaded StaticMeshActor_49 mesh,
  `/Engine/BasicShapes/Cube.Cube`, already proven in packaged preflight snapshots. Log records
  center(-1300,-150,185), scale(.9,.9,.9), camera-to-target trace per active tick. All ten
  onset frames join by captured frame_index to occluder_hit1 AND target_pixels0. Every onset
  empty_region, prev_target_pixels=-1, phase indeterminate; zero measured pairs and five final
  records per order. Original B1 passes. No offscreen lever or geometry exemption.
- Long blinking runs exercise more than8 phases with stored detail cap8 and actual phase_count.
  Short events require only2 pairs per phase. Target-drop and dedicated run-end each produce
  their required finalization reason in both orders.
- **Actual OpenLevel teardown passes both orders**, change_teardown_flush1 and finalized_by
  teardown, closing that previously mandatory unrun gate. The world-cleanup unlock log has
  controller=None because that controller is already gone: ownership cleared; it does not prove
  a decrement on a surviving controller. Live explicit unlock and ordinary run-end restoration
  are separately exercised. Separate-run stale-work/world-replacement limits remain below.

Two invalid capture attempts were retained and rejected before reading measurement outcomes:
V2_RUNEND_N_A1 foreground39/41, then A2 passed42/42; V2_TEARDOWN_S_A1 foreground40/41,
then A2 passed47/47. All other accepted gates used attempt1. No fourth attempt or outcome retry.
Before revision2, the four D0867240 static legs also passed first try (5 events/20 pairs each,
measured19/19/18/19); final-build evidence above supersedes their binary qualification role.

## Disclosed offline auditor correction

The first occluded run initially stopped the wrapper on its inherited normal-case minimum
of3 **legacy** events. The existing m26 measured-zero veto removed4 legacy records, leaving1;
all5 sidecar final events remained. The approved G-OCCLUDED role requires empty-region refusal,
not positive legacy certification. Plan G-OCCLUDED and the existing G11 demonstration distinction
support treating this as an auditor-scope fault. Corrected only that branch to require>=3 sidecar
finals and exact legacy+veto accounting(1+4=5), keeping B1/foreground/trace/onset checks intact.
Re-audited the SAME raw launch; no physical retry or source/build change. Original failed audit,
results and helper are preserved under081-11-evidence. Ordinary legacy event minima unchanged.

## NoHold failure: bounded cause read and remaining work

`V2_NOHOLD_N_A1`:90 captured frames, B1 valid before capture and exact zero-rotation calibrated
views throughout,86/86 foreground, live run-end unlock. No handled-ensure. Bench flag logged ON,
streaming1/UseAllMips0. Each of five Apply attempts matched the Cube component but found
**0 candidate textures**, returned not applied; summary says five zero-match attempts and no fires.
Sidecar has0 pairs/0 final events. The required minimum3 final events therefore fails.

Source Anomaly_StuckLowMip.cpp first builds the eligible texture list; NoHold suppresses the
streaming-bias write ONLY for such a held texture. It does not manufacture a texture or active
fire when that list is empty. Thus absence of final records is expected for these failed Apply
attempts; this run cannot test no_labelled_frames and cannot be credited as its expected zero.
This differs from the occlusion audit correction: the required sidecar events themselves do not
exist. Cause/evidence:081-11-evidence/nohold-cause-read.json and the preserved full launch/bank.

Held remainder: corrected NoHold natural and synthetic; moving null/solid natural (fixture built,
never invoked); mandatory six-leg AEBD09EA legacy comparison (helper prepared, never launched).
Synthetic moving legs remain optional if time. Lyra scoped natural twins+G3/G270 remain last,
zero attempts used, first-leg three-invalid auto-drop unchanged. No Lyra cost legs.
Stage3 requires GREEN R2–R4 plus R5 resolved; neither release condition is claimed here.
Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4** verbatim; cost/oracle gates remain UNRUN.

## Coverage limits: prior text preserved verbatim

The historical text below is retained for continuity. Dated updates immediately afterward prevent
its old UNRUN statements being mistaken for this round's disposition.

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


### 2026-09-22 amendments to that historical text

Stage2 implementation, static twins, delay3, hide/occlusion/phase/short/drop/run-end and actual
teardown now have evidence above. Actual teardown passes on finalA699E9EE in both orders;
separate-run stale-work transition and real owner viewport/world replacement remain unexercised.
Producer ramp and Stage3 selftest/oracle/cost remain unrun. NoHold gate is failed-precondition,
not exercised zero-label evidence. Moving twins, legacy comparison and scoped Lyra remain unrun.
Stage1 accepted36; other24 Lyra G1–G12/order legs **DROPPED-BY-OWNER-DECISION**, not FAIL or
UNEXERCISED. Historical Lyra L1-invalid attempts and old G14 failed-precondition record retained.

## Boundary and evidence read order

Read this full report, latest081-10 ruling, frozen081-11 journal, final-ledger.json,
validation-v2-results/summary, accepted-gate-readings, suite-v2-results, NoHold full logs/cause,
then individual fixture/measurement audits and original failed occlusion audit. binary-v1/v2,
six build logs, bank-manifest, evidence-manifest, frozen manifest and postflight pin the evidence.
All files are under D:/IntrusiveAnomalies/_reviews/081-11-evidence unless a bank path is recorded.
Bank manifest hashes4,463 files from all29 capture attempts, including both focus invalids and
NoHold.16 no-capture launches plus29 captures =45 launches this round. Predictions preceded legs.

Feature commits pushed, source/docs frozen; final feature hash and restoration proofs live in
postflight.json. Main restored m51 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67; Lyra detached
caa68c6799db023f7a24c0d6cfc56124ce27b542 and original9 module files restored; exactly its two new
AnomalyBench files removed. Main Editor modules and built/staged Game remain A699E9EE-era
candidate binaries despite restored m51 source. CaptureBench, master/m51 refs, old archives,
containers, prior artifacts and existing untracked files preserved. All CURRENT bytes retained,
including separate Blender/video histories. No owned engine/build/capture remains in flight.
No merge/tag/cook/office transfer/owner-app changes. No fresh session advised for this workstream.

## Questions for chat

Please rule on the failed NoHold precondition and continuation: use an already-cooked textured
target with proven eligible texture(s), predeclare its targeting/B1 scope and demonstrate an
active NoHold fire before claiming zero labelled phases. Preserve this zero-match launch;
natural has two nominal attempts unspent, synthetic three. Then complete both NoHold orders,
moving twins and six-leg legacy comparison, with R5 last. This report proposes no allowance
reset or waiver of the required events. The22 accepted final-build gates and5/5 qualification stand.

VERDICT: NEEDS-DECISION
