# 081-10 — fixture input lock and StackOBot placement

Authority: owner supplied081-09-chat-ruling-fixture-fix.md. Chat accepts the shared failure
signature as sufficient for R1 step3; no more old-binary diagnosis repeats. Codex implements;
Chat orchestrates; Code reviews after Stage3. Four-hour bound; this block ends with081-10 report.

## Predictions before implementation/build/validation

F1 default-off bench input-lock command: explicit fixture flag, CB_GateLevel/L_ShooterGym,
Game/Editor non-Shipping targets only. Call SetIgnoreLookInput(true) and SetIgnoreMoveInput(true)
once on the ready controller before settle; repeated enable must not stack increments. Retain
until run end; release only our increments, preserving any pre-existing host locks. Explicit
off outside capture, world cleanup and module shutdown also release and log. Controller change
is logged and refused rather than silently certifying a new controller. No host source change.

F2 extend the existing one-shot measured-camera-offset placement to CB_GateLevel. Desired camera
origin(-1500,0,260), rotation(0,0,0), from banked R1V3_A1 snapshots/result and original B1 calibrated
bbox(0,485.2,306.1,234.8) in CaptureBench/tools/a54_oracle.py. Lyra reference unchanged. Preserve
camera/movement implementation; no ghost mode, replacement camera, velocity reset or persistent
pose pin. F1 changes input acceptance only. Place once after fixed five-second settle and independently
sample B1 afterward. Protocol acknowledgement alone is not successful placement or a pose check.

Build Game, StackOBotEditor, LyraEditor Development, no cook. Archive2C21EDE5 remains byte-identical.
Stage/hash/archive fixed binary and module files; A44 literal input-lock/placement tokens plus
existing positive controls and invented absent token. No asset/shader/measurement arithmetic edit.

Validation: fixed sequence ON1,OFF1,ON2,OFF2,ON3,OFF3,ON4,ON5 (alternating while controls remain).
ON locks input at world-ready before five-second settle, then places once; OFF issues neither.
Same launch/render/geometry recipe as081-09. Collect90 fresh snapshots per launch,20Hz request;
no capture_start, injection or measurement run. Predeclared ON success requires all5/5 complete,
settle SAMPLE0, modal rotation0/0/0,100% modal bbox coverage, unchanged B1 PASS, observed exact
calibrated camera origin and placement-success/input-lock logs. Check unlock logging afterward.
Foreground PID/sample counts retained; zero-foreground preflight CAN PASS per new ruling.
OFF controls have no required pass/fail outcome; retain every reading. No B1-result retries.
If ON<5/5, stop/report without renewal. Selftests and raw snapshots remain distinct from captures.

F3 unchanged focus requirement for future MEASUREMENT legs: foreground throughout, with PID and
sample-count audits. Input lock prevents a focus transition moving the pose; it does not waive
A63. The081-11 runner must include these controls/audits and re-run four static twins on the new
binary. Historical twins stay accepted; old DELAY_N invalid attempts stay uncredited.

On5/5, new three-attempt allowances are released as ruled, but stop at this081-10 report/ferry
boundary before081-11 R2–R4 and081-12 R5. R2/R3 occluder/motion mechanisms remain authorized,
actual teardown and six-leg AEBD09EA invariance mandatory. Lyra last, scoped twins/G3 and
first-leg auto-drop unchanged. Stage1 accepted36, other24 Lyra DROPPED-BY-OWNER-DECISION.
Carry FPS21.5 vs30 on evidence-OFF MAIN4, prior coverage limits, and all unrun gates.

**Chat review NOT REQUIRED** for this authorized correction/build/preflight. **Chat review
REQUIRED** at081-10 report disposition before the next bounded block (bounds as before).
Stage3 requires GREEN R2–R4 plus R5 resolved. No merge/tag/cook/office transfer/owner-app changes.
Restore m51/Lyra entry source/modules, preserve all CURRENT history including unrelated work.
No fresh-session recommendation for this continuing plugin workstream.

## Outcome — first enabled preflight refused, matrix stopped

Built source4a1475670bca25d3e6735a137547637bb986482a; all three Development targets succeeded.
GameB283E34C full SHA256 b283e34c13a8229680848e6b87ac5e448b457a2e0f0e5ae8a15759742d0384da,
241723392 bytes, built/staged/archive agree. Lyra eleven-file archive
_binary_baselines/m55-stage2-fixture-lyra-23258325. Pre-fix2C21EDE5 and older archives preserved;
five cooked containers unchanged. A44 input-lock/placement + existing controls positive, invented
token absent. Only fixture module, descriptor and typed control bridge changed; arithmetic unchanged.

F12_ON1 first enabled launch: server/world ready3.332s, authenticated, module command entered,
then IAI-INPUT-LOCK REFUSED fixture/map/ready-controller/inactive-capture/argument gate.
Observer stopped because no successful lock log existed. No settle, placement, snapshots or
capture. No retries. Four remaining ON and all three OFF launches UNRUN, zero qualified of five.
Runner status PROTOCOL-FAIL is the wrapper; substantive outcome is fixture command refusal.
The ruled <5/5 stop applies, no allowance renews. Owned PID35828 stopped/joined by finally.

Own implementation issue: InputLock unnecessarily requires PC->GetPawn(), even though ignoring
input requires only the controller. PlaceView also assumes GetPawn rather than inspecting the
active view owner/spectator. Runtime map log says No Spawnpad found. This strongly motivates the
readiness correction but the combined refusal log cannot prove which pointer was absent; do not
claim pawn absence is directly measured. Engine exposes GetPawnOrSpectator separately. Need exact
controller/pawn/spectator/view-target diagnostics and controller-only lock gating, then safe placement
of the actual view owner under existing scope. No new camera, host edit or pose pin is proposed.
No follow-up implementation, second build or preflight was made after the ruled stop.

Input-counter balancing, run-end/controller-change/cleanup release paths are implemented but
runtime UNEXERCISED; first enable refused. Explicit unlock also UNRUN. Dynamic module loading
did work: module emitted its refusal despite the old cooked descriptor. Independent B1 remains
unchanged and unrun on the new candidate, never inferred from an executed command receipt.

G278 records Chat's accepted focus-transition mechanism and fixture rule. This failed local
readiness assumption does not reopen the old-binary diagnosis. Stage1 accepted36/Lyra24
DROPPED-BY-OWNER-DECISION unchanged; old DELAY_N three invalids remain uncredited, zero remaining.
R2–R4/new static twins, actual teardown, AEBD09EA invariance and R5 still UNRUN. F3's whole-leg
foreground audit remains mandatory for081-11 runners; no measurement runner was exercised here.

**Chat review REQUIRED** for this failed081-10 validation disposition and correction/revalidation
before081-11. **Chat review NOT REQUIRED** for completed authorized build and safe documentation.
Next mandatory checkpoint is Chat's ruling on081-10. Stage3 held; no new allowances or release.
No fresh session recommended. Preserve all earlier CURRENT bytes, including separate workstreams.
