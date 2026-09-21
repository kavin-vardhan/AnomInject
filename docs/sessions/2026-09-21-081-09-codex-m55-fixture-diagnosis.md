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
