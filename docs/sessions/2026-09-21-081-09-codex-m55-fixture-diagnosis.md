# 081-09 — Stage 2 R1 fixture diagnosis

Authority: owner supplied 081-08-chat-rulings-stage-2-continuation.md on 2026-09-21.
This bounded block is R1 only. Stop/report for owner ferry before R2–R4; Lyra runs last in R5.
Codex implements; Chat orchestrates; Code reviews after Stage 3. No mailbox brief.

## Prediction and recipe before any launch

Compare five launches of archived Stage 1 AEBD09EA (A) and five of Stage 2 2C21EDE5 (B),
in fixed A1,B1,A2,B2,A3,B3,A4,B4,A5,B5 order. No capture_start, IAI.Capture.Start,
manual anomaly injection, placement or scene mutation. No retry/replacement of invalid launches.
Same staged executable path and five cooked containers; hash archive/staged before each launch.
Restore Stage 2 staged executable in finally. Single owned PID, idle box before each launch.

Use CB_GateLevel, windowed 1280x720, same AE-off/AA0, CaptureBench.Marker1, stall0,
SVE1/Delivery0/Pace1/Fps30/Config2,4,16,4,0, ChangeEvidenceCases1/ChangeGate14/SynthTickOrder0,
-IAIBenchFixture/-unattended/-nosplash. Start only the existing control server. Commands absent
in A are recorded as unavailable, not invented overrides. The diagnostic difference from the
original capture legs is deliberate: server snapshots replace capture, with no active anomaly.
Force/verify foreground as the existing owned runner does; no cursor pin or synthetic mouse motion.
Record cursor/foreground state for diagnosis, but do not turn these into post-hoc validity rules.

After server/world readiness, fixed five-second settle; subscribe at20 Hz and retain90 fresh
camera/visible-actor snapshots. B1's settle algorithm runs on that sequence (K5,0.5 degrees),
then its unchanged >=90% modal coverage, <=3 distinct rounded boxes, eight-pixel calibrated-bbox
tolerance. Report settle SAMPLE index explicitly: no capture index exists in this experiment.
No settled sample or missing target/invalid geometry fails closed. No threshold adjustment.

Snapshot rect is normalized min/max of visible StaticMeshActor_49's static mesh component;
multiply by viewport dimensions and subtract min from max to obtain the B1 bbox. StaticMeshActor
has one static mesh component; ControlSnapshot/AnomalyViewport's eight-corner clamped projection
matches the label writer's clamped actor-bounds projection on this fixture. Both source paths
are unchanged between A/B. Require valid1280x720 view and exact target; never insert the reference
bbox for missing data. Adapted B1 rows are named control-b1-rows, never labels/capture artifacts.
Every snapshot must have capture.running=false, framesWritten=0 and no active injection.

Hypotheses before readings: AnomalyBench is Editor-only and is absent from the packaged Game;
its constructor cannot explain a Game start. Twin registration adds catalogue entries but neither
constructor places a pawn nor applies an anomaly. New measurement code is idle without capture.
Thus I predict A/B will be similar under this preflight; a B-only failure would contradict that
prediction and require the authorized flags-first bisect. Similar failures would support host/
fixture non-determinism and release the authorized L2-style StackOBot placement correction.
If both are5/5 valid, that is evidence only about settled no-capture starts, not proof that the
capture-start path or earlier invalid attempts are fixed; report that uncovered case without
claiming a cause or silently renewing allowances. If rates are ambiguous, do not force a branch.

If needed, record correction prediction BEFORE rebuild or additional preflights. No rebuild,
placement or new allowance is justified solely by this prediction. Fresh three-attempt Stage 2
allowances require the ruling's corrected final binary >=5/5 B1 preflights. Prior invalid attempts
remain uncredited, four accepted static twin legs remain accepted. Stage 1 accepted36 and Lyra24
DROPPED-BY-OWNER-DECISION unchanged. R2/R3 fixtures are authorized in the next block, not run here.

**Chat review NOT REQUIRED** for this authorized R1 diagnosis/fix within the ruling.
**Chat review REQUIRED** at the 081-09 R1 report before proceeding to R2–R4, per the explicit
stop/ferry boundary. Stage 3 held until GREEN R2–R4 plus R5 resolved. No fresh session recommended.

## Tooling correction before the usable A/B series

R1_A1 launched AEBD09EA but collected zero snapshots. The new client incorrectly demanded
a WebSocket Text frame; the existing UE NetworkingWebSocket sends JSON in binary frames.
Authentication succeeded; the framing check then threw. This is PROTOCOL-FAIL, not a B1
reading or fixture result. PID was joined, log/launch retained, candidate restored. Correct
the reader to accept UTF-8 JSON in either data-frame type (close still fails), as the existing
client does. Start the fixed ten-launch series with distinct R1V2 labels, same predeclared
recipe and order; no unavailable pose reading is reconstructed or counted. No capture occurred.

## Second observer correction before a complete paired series

R1V2_A1 collected90 snapshots: B1 valid (pitch359.36, bbox0/477.9/307.7/242.1).
R1V2_B1 then stopped before snapshots because foreground was not held at the five-second check.
Preserve both pilot launches and the partial result file. The new observer omitted the existing
runner's WScript.AppActivate call alongside Win32 foreground forcing; restore that same mechanism.
Also record foreground loss without aborting before diagnostic snapshots: R1 needs the pose
and B1 data even when focus fails. This is not a qualification waiver: report raw B1 separately
from focus, and do not certify a final fixture from a focus-invalid preflight. Fixed recipe and
five-second settle remain. Start complete R1V3 A/Bx5 with this one observer on both binaries.
The three pilot launches are retained separately (one framing failure, one B1 reading, one focus
failure); they are not result-based retries of a Stage 2 measurement leg. No B1 threshold changed.

R1V3_A1 valid and R1V3_B1 invalid are now fixed observations and will not be rerun/replaced.
A2 observer then read a newly created but still empty log as null and threw before server/world
readiness. Cast that read to string (empty is not ready), retain A2 as PROTOCOL-FAIL with zero
snapshots, and continue from A2R1 then B2…A5/B5. This correction does not change sampling or B1.
Partial V3 results are appended, never cleared; invalid B1 readings remain in the denominator.

## Final diagnosis — NEEDS-DECISION, no correction branch selected

The complete R1V3 series has900 snapshots, five observations each A/B in alternating order.
Stage 1 B1-valid4/5, Stage 2 B1-valid2/5. Sustained foreground90/90 samples: A2/5, B0/5.
The same counts2/5 and0/5 meet both B1 and sustained focus; raw B1 passes without focus
are not a final-fixture certificate. Four raw B1 passes late in the series had0/90 foreground.
Both binaries show orientation-only instability; origin(-1500,0,260), FOV90 and1280x720 are
constant across all900 snapshots. All have capture idle, framesWritten0, empty runDir/active.

Foreground PID18684 dominates failed/late observations; a read-only post-series lookup resolves
it to SearchHost. No owner application, window, process, input setting or search UI was changed.
This identifies foreground contention, not its initiating action and not a causal proof of every
earlier failure. B1 failures occur on both binaries, so a Stage-2-only regression is not established;
1/5 versus3/5 pose failures with inconsistent focus does not establish similar rates either.
Follow the predeclared ambiguous-result branch: no flags bisect, L2 Game change, rebuild or new
qualification allowance. Cause of the earlier Stage 2 capture failures remains NOT ESTABLISHED.

All14 launches retained: ten full comparison observations plus four ancillary (binary-frame
observer failure, one earlier valid A pilot, foreground failure before sampling, empty-log observer
failure). The V3_A2R1 launch only replaces an observation absent due to observer failure; no B1
failure is rerun or removed. Explicit tooling corrections above precede their further launches.

Report081-09 contains every reading and asks Chat to resolve the focus-confounded branch, ideally
a bounded comparable-focus preflight before attributing a binary regression or permitting L2.
The accepted static twins remain accepted; DELAY_N still has0 remaining qualification attempts.
R2–R4 authorized mechanisms remain deferred to their own post-R1 block; R5 Lyra stays last.
Stage 1 accepted36/Lyra24 DROPPED-BY-OWNER-DECISION unchanged; actual teardown, runtime invariance,
Stage 3 oracle/cost still open. Preserve FPS21.5 vs30 on evidence-OFF MAIN4 and prior limits.

**Chat review REQUIRED** at this R1 disposition/next step before any further preflights/correction
or R2–R4, because the observed rates/focus do not select the ruling's conditional branches and
the ruling explicitly requires this report/ferry stop. **Chat review NOT REQUIRED** for completed
authorized diagnosis and boundary documentation. Stage 3 held. No fresh session recommended.
