# 081-11 — guard correction and revalidation

Authority: owner supplied 081-10-chat-ruling-fix-guard-and-revalidate.md.
Codex implements; Chat orchestrates; Code reviews after Stage 3. Four-hour bound
starts at preflight.json utc. No mailbox brief, host source changes or cook.

## Prediction and fixture rules, before implementation/build/legs

Iteration 1 corrects the unnecessary possessed-pawn prerequisite in InputLock.
A ready controller suffices. PlaceView first uses the view target if it is a pawn
(including spectator), otherwise GetPawnOrSpectator, otherwise no_view_owner.
Every fixture command logs controller/pawn/spectator/view-target readiness before
deciding; refusals identify one failed condition. Keep flag/map/inactive gates,
owned input counter balancing, one-shot camera-offset placement, no substitute
camera/pawn or persistent pose pin. No Spawnpad found is retained as a pre-existing
host-map warning, not a repair target. CB calibration remains (-1500,0,260), zero
rotation, as cited in 081-10; Lyra calibration is unchanged.

Build Game, StackOBotEditor and LyraEditor Development; archive the new candidate.
Preserve B283E34C refused candidate and all older binaries/evidence/containers.
Sequence ON1 OFF1 ON2 OFF2 ON3 OFF3 ON4 ON5; 90 snapshots each; no capture/injection.
ON requires unchanged B1, settle sample 0, modal rotation 0/0/0, 100% coverage,
calibrated camera, exactly one lock and placement, explicit unlock restoring prior
flags. Foreground is diagnostic only here; OFF pose/B1 readings have no pass gate.

Executed sampling pose/B1 failures count against 5/5 and stop the round. Bench
readiness refusal, runner protocol fault, or engine handled-ensure before sampling
is HARNESS-INVALID, counted neither as a pass nor a failure of 5/5. At most two
fix-rebuild iterations per round, each cause journalled; stop if the harness cannot
execute within this bound. Retain every invalid launch; never replace a B1 result.

On 5/5 every Stage 2 leg gets a fresh three-attempt allowance; accepted old twins
remain accepted, old delay invalids remain uncredited. Continue R2-R4 in this same
round if the four-hour bound permits, including new-binary twins, real occlusion,
natural moving twins, actual teardown and six-leg AEBD09EA comparison. Measurement
legs still require whole-leg foreground PID/sample audits. Lyra runs last, scoped
natural twins plus G3/G270; first-leg three-invalid auto-drop unchanged. Otherwise
081-11 reports validation, 081-12 R2-R4 and 081-13 Lyra. Stage 3 remains held pending
GREEN R2-R4 and R5 resolved. Carry FPS 21.5 vs 30 on evidence-OFF MAIN4, all prior
verbatim coverage limits and unrun gates. Stage1 accepted36 / Lyra24 dropped stays.

**Chat review NOT REQUIRED** for this correction, validation and successful same-round
R2-R4 continuation. **Chat review REQUIRED** at a failed executed validation or the
Stage 2 report checkpoint. Restore entry source/Lyra modules at the safe boundary;
preserve all CURRENT history. No merge/tag/master/m51 edits or owner-app changes.


## Validation result and R2-R4 continuation prediction

Iteration 1 built all three targets exit0. Source9a28f0b; GameD0867240, archived,
with eleven Lyra module files. ON1/OFF1/ON2/OFF2/ON3/OFF3/ON4/ON5 all completed.
All five ON passed strict B1/settle0/zero rotation/coverage100%, lock/place/unlock;
all eight observations had90/90 foreground. Readiness proves pawn=null and the
active view owner is the SpectatorPawn. Three OFF readings also passed raw B1.
No HARNESS-INVALID launch in this iteration; no second correction used.
Fresh three-attempt allowances now released. Old evidence remains unmodified.

Next run four static twins NULL_N NULL_S SOLID_N SOLID_S on D0867240, 90 frames,
1280x720 AA0/AE-off, seed777, Config2/4/16/4/0, ChangeGate14 CRC receipts.
New runner authenticates, locks input, settles5s, places once, observes90 snapshots,
and checks unchanged B1 BEFORE capture_start. Hold lock through run; require run-end
release. Foreground PID sampled at50ms from capture_start through summary/tail;
any sample outside owned PID invalidates the attempt, independently of measurement.
Every attempt retained, maximum3 per leg. Missing protocol/artifacts, runtime ensure,
failed B1 or absent unlock also invalidate. Static capture labels receive the original
check_pose gate as well. No frame or measurement result chooses retries. Once fixture
valid, run same Stage2 audit: identity/conservation/CRCs/final records/caps/counters;
expect null near zero and solid onset1.0, controls reported, never subtracted.
If a gate fails valid evidence, or allowance exhausts, stop and report with cause read.
All remaining authorized R2-R4/R5 gates remain pending until actually run.
Chat review NOT REQUIRED for this same-round continuation; next mandatory checkpoint
is failed gate/exhaustion or completed R2-R4 report. Stage3 remains held.


## R2/R3 remaining fixture implementation prediction (before second build)

D0867240 four static twins all accepted first attempt;5 events/20 pairs each,
measured19/19/18/19; all foreground samples held, original capture B1 passed.
Remaining gates need two authorized fixture mechanisms and delayed-capture args.
Build revision2 adds these default-off bench mechanisms; it is not a repair of a
failed qualification result. Conservatively count it as the second build iteration.
Requalify its binary with the same5 ON/3 OFF matrix before any measurement, and rerun
all four static twins on that final identity. Preserve D0867240 and all its evidence.

R2: duplicate StaticMeshActor_49's already-loaded Cube StaticMesh and materials.
Cooked presence is proven by that actual actor/component/asset in all eight packaged
preflight snapshots; log full asset path from the packaged mesh at construction.
Use the component rotation and0.75*component scale, center halfway from camera to
original bounds center (same mesh, therefore1.5 times target angular span). Actor
remains untagged, query collision blocks Visibility, no host source/cook. Invoke only
after settled B1 before capture. Log camera-to-target-bounds-center Visibility trace
on every active world tick, including GFrameCounter, hit actor and whether occluder.
At each onset require matching captured frame_index trace hitting occluder AND
occlusion-correct target_pixels0. Require empty_region/indeterminate/prev_target=-1,
zero measured phase pairs, final events. Static B1 still applies; no offscreen lever.

R3: explicit bench YawRate fixture, -3 degrees/sec control yaw while capture active,
armed only after settled B1. No player input or replacement camera; start after B1,
stop at run end. Natural null/solid90-frame legs only. Require cam_moved on>=90% of
window pairs with nonzero quantized rotation/position magnitude; report control rates
without subtraction and expect null onset far below solid. B1 applies before motion.

Typed scene-fixture request accepts only occluder or motion, same bench gates; every
fixture command logs readiness before decisions. capture_start's benchDelayFrames=3
is accepted only non-Shipping explicit named-map fixture solid_swap, passed as delay=3
through the existing StartRun argument path. Other requests retain empty args.
Remaining recipes use prior081-08-suite.py: normal/depth hide, delay3 both orders,
occluded onset both orders, blink>8 both, short both, drop both, runend both, actual
teardown both and nohold both. Delay's transition must appear at window index3;
depth hide must log drawn=count while legacy observable remains unchanged. Natural
moving twins then mandatory six-leg AEBD09EA legacy comparison. All gates max3attempts,
foreground throughout; valid measurement failures stop, never result-based retries.
No claims for unrun gates. Existing four-hour bound and review checkpoint unchanged.


### Source-backed recipe clarification before V2 measurement

R2 uses missing_object, whose default unscoped matching accepts the already-occluded
actor; null_effect's Apply always filters visible components and is unsuitable for
this gate. No production matching changes. Both orders use the same settled B1 and
original capture-label B1 (no geometry waiver). NoHold has no labelled bbox by design:
apply unchanged B1 to90 fresh pre-capture target snapshots, then require all captured
views retain exactly calibrated origin/rotation. The post-label check is reporting-only
there because no labelled target rectangle exists. Moving legs use the explicit R3
settle-only B1 rule. Other static legs require the original capture-label B1 as well.
Second build all three targets exit0, sourcecb14f31, candidateA699E9EE; revalidation
is running before measurement. Core change arithmetic and label serialization unchanged.


## Offline audit correction after first already-occluded leg

V2_EMPTY_N_A1 had valid preflight/captured B1,87/87 foreground,5 final events,
20 refused pairs/0 measured; every onset empty_region,prev_target=-1,target_pixels0
and same-frame trace hit IAIBenchOccluder. Asset logged /Engine/BasicShapes/Cube.Cube,
center(-1300,-150,185),scale(.9,.9,.9), copied from actual cooked actor_49.
The wrapper nevertheless stopped on its inherited normal-case legacy>=3 check.
Existing m26 veto removed4 MEASURED_ZERO events from annotation.json, leaving1;
all5 sidecar finals correctly survived. The approved G-OCCLUDED prediction is an
empty-region/indeterminate demonstration, not positive legacy certification; plan
lines709/950 explicitly require empty region. G11's existing log distinguishes
veto demonstrations from certifying legs. Requiring3 non-vetoed legacy events here
contradicts this gate's predeclared role. Correct ONLY this auditor branch to require
>=3 final sidecar events plus exact legacy+veto accounting (1+4=5); preserve the
original failed audit and helper. Re-audit SAME raw launch, no replacement/capture
retry or source/build change. All B1/foreground/onset/trace/counter checks unchanged.
This is an offline checker-scope fault, not a failed pixel outcome or third rebuild.
No blanket relaxation of legacy event counts for normal legs. Chat review NOT REQUIRED
for this source-backed audit repair; disclose it in the Stage2 report checkpoint.


### Legacy comparison runner details, before any legacy launch

Six unchanged MainWorld recipes from081-08-legacy.py: AEBD09EA OFF/OFF, final
candidate OFF/ON/ON/OFF, blinking banked MainWorld target,90frames,720p,AA0,
Config2/4/8/4/0, seed777. No CB-only input-lock/placement commands on either side.
Keep original startup A63 proxy and no off-target B1 certificate; compare all
camera rows as an explicit invariance output alongside masks/counts/keys/onsets/
event ranges/MASK-TIE. Add raw foreground samples every50ms from launch until
run_summary and identify measurement interval from labels.t_wall. Match clocks
using QPC/frequency+16777216, exactly WindowsPlatformTime.h:29. Require every sample
within [first_frame_wall-.05,last_frame_wall+.05] belongs to owned PID; nonempty
sampling interval required. Read camera/focus validity before invariance outcomes.
Max3 attempts, keep invalids; owned-PID cleanup in finally and candidate restaged
in comparison finally. A missing/nonmatching reading is not pass. This comparison
remains mandatory and is still UNRUN until the preceding gates complete.


## Final outcome and disposition

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


Final source cb14f3109d56d9b6b25ab4ccb72472b5c0f2eae8, Game a699e9ee46d43a860912977cc8ed721cf8233958398f9cab9b0f2b43bf36a46d.
Two builds,16 preflight launches,29 captures (4 earlier-candidate,25 final-candidate).
22 accepted final-build gates,2 focus invalids,1 NoHold failed-precondition.
Full report _reviews/081-11-codex-m55-guard-and-stage2.md; final ledger/evidence manifests
record all paths, hashes, counts and unrun gates. Explicit preflight unlock restored flags;
ordinary run-end release exercised; teardown cleanup cleared ownership after controller loss.
No surviving-controller decrement is claimed from controller=None teardown logs.

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


Safe boundary restores main/Lyra source and original Lyra modules; main candidate binaries
remain mismatched to restored m51 source. Preserve all CURRENT history and prior artifacts.
Feature pushed, source/docs frozen, no owned processes. Chat review REQUIRED for NoHold
recipe correction and remaining gates; Stage3 held. No fresh-session recommendation.
