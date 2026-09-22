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
