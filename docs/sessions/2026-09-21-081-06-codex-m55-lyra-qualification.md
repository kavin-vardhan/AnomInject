# 081-06 — m55 Stage 1 Lyra qualification (2026-09-21)

Authority: `_reviews/081-05-chat-rulings-stage-1-qualification.md`. Implementation/source 3dc1dda,
feature docs aa99a10, staged Game AEBD09EA and archived Lyra module set m55-stage1-lyra-74598de2
are accepted as built. No source change or rebuild planned. Stage 2 remains held pending the matrix.

## Rulings and carry-forwards

The banked MainWorld strict reduce invariant passes. Chat redefines cross-run CB_GateLevel mask
comparison to identical occupancy AND each nonzero byte equals its own row's mask_value; the prior
71/71 result passes this rule. Raw stencil-tag numbers are not cross-run identifiers. Whole-artifact
EXTRAS are accepted as timing/float noise. Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4** verbatim
into Stage 3: any cost leg at that FPS needs an explanation before the gate can be read. Retained-byte
high-water priors remain 17.5 MB StackOBot and 19.4 MB Lyra. Teardown/phase/event gates remain Stage 2;
the full prior coverage-limit block is copied verbatim to `_reviews/081-06-evidence/coverage-limits-verbatim.md`.

## Bounded cause read (before reruns)

The older visible V5 G14 synthetic and failed V6 G14 synthetic log the same map and effective launch
arguments (apart from output/log names); both load `/ShooterCore/Maps/L_ShooterGym` and contain
pawn-avatar/respawn events before and after capture. The runner has no pose setter and uses the same
45-second settle. The logs do not identify a selected PlayerStart or prove a stale/saved camera cause.
Why the starting view changed is **not established**. No runner configuration difference was found to
fix; enforce the newly ruled L1 instead. Command-line/map/respawn excerpts and line numbers are in
`bounded-cause-read.json`; no deeper cause investigation is undertaken.

## Prediction and procedure (before qualification legs)

L1 reads a fresh control-server snapshot before sending capture_start, through the existing snapshot
channel (no engine/plugin change). The camera uses the same GetActiveViewInfo source as labels.
Reference: older visible healthy `M55S1V5_LYRA_G14_S`, labels.jsonl line 1, session_index 0:
origin (-443.9807434082031,-70,212.00010681152344), pitch/yaw/roll
(0,0,-0.08256798918660047). Exact file, SHA and line are in l1-reference.json. This is the
line-oriented pose log in the capture artifacts, not a fabricated UE text-log pose line.
Require valid finite snapshot, viewport 1280x720, capture not running, position distance <=150 cm
and shortest quaternion rotation distance <=10 degrees. Geometry and protocol errors halt rather
than become result retries. L1 pose failure is INVALID-FIXTURE before capture; retain its snapshot,
computed distances and runtime log, stop only the owned PID, permit at most three total attempts per
leg (shared cap with handled-ensure invalidity). A passing L1 followed by fewer than three events or
empty/vacuous G14 masks is FAIL and is not retried. Camera motion after capture starts is allowed.
Unsubscribe the snapshot channel before capture so this preflight observer does not run during it.

Run G14 synthetic first, then G1–G12 both orders on the exact accepted Lyra modules. Keep the prior
failed G14 synthetic as failed-precondition-undefined, not credited/deleted. Reuse the accepted
30 StackOBot and five prior final-source Lyra legs without rerunning or pretending they had L1.
Expected lifetime assertions remain the 081-05 gate predictions. G11 retains its explicit 9-frame
lifetime-only exception; all ordinary legs require >=3 events. Preserve legitimate empty-region,
budget and dependent refusals as readings. Halt on any real assertion failure or three invalid
fixture attempts. No post-result pose adjustment or target replacement.

**Chat review NOT REQUIRED** for this released qualification. A GREEN report automatically releases
Stage 2 under 081-03; Chat still reads it, and the next mandatory Chat checkpoint is Stage 2's report.
Any new finding or non-GREEN qualification requires **Chat review REQUIRED** before continuation.
Report `_reviews/081-06-codex-m55-stage-1-lyra-qualification.md`; ~4-hour bound. Restore entry
checkouts/modules at the report boundary; no merge/tag/master/m51 ref change or mailbox write.
