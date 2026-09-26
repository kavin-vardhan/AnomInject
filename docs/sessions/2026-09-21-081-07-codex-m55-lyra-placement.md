# 081-07 — m55 Stage 1 Lyra L2 preflight (2026-09-21)

VERDICT: NEEDS-DECISION

Authority: `_reviews/081-06-chat-ruling-lyra-placement.md`. Codex implements, Chat orchestrates,
Code independently reviews after Stage 3. No mailbox brief. Accepted source is 3dc1dda;
Game AEBD09EA and archived Lyra m55-stage1-lyra-74598de2 are unchanged.

## Completed bounded step

Read the new ruling and prior qualification evidence; rechecked the restored checkouts, archived
candidate and live entry modules. Inspected accepted-source control-server dispatch, the plugin's
console surface and CaptureBench source. Source/Shaders at feature entry 4662e6f are identical to
the accepted implementation 3dc1dda. Frozen 112 source files and a line-numbered command/setter
inventory support the negative finding; no live qualification attempt was needed or started.

The control server has no placement or generic exec message. Unknown message types receive an
ack, so an invented set_view request could look successful while doing nothing. Existing
TeleportTargetOffscreenAt moves anomaly targets during capture, MaskPairingProbe moves its own
probe, and Letterbox changes aspect; none places the pawn/view. CaptureBench has no placement
setter. Engine BugItGo calls Ghost before and after pawn teleportation; it changes movement and
does not establish the requested camera-origin placement or the fixture-only gate. It was not run.

L2 is technically implementable without Lyra host source changes, but the new compiled console
lever requires a rebuild. That conflicts with this ruling's unchanged-candidate/no-rebuild clause.
Stopped before source edits/builds or attempts. This is a mechanism/authorization conflict,
not evidence that host changes are necessary and not a product failure.

## Preserved matrix and follow-up

36/60 accepted: StackOBot 30, Lyra G13–G15 both orders 6. G1–G12 both orders remain UNRUN.
All 24 fresh L2 allowances remain 3/3. Old G1-native three INVALID-FIXTURE attempts and original
failed-precondition-undefined G14-synthetic remain retained and uncredited. G275 records L2 as a
rule awaiting implementation, never a measured pass. No capture, cook, rebuild, module swap,
host source edit, merge, tag, or mailbox write. Documentation-only feature commit is permitted.

Proposed next step for Chat: release a fixture-only placement lever build with explicit binary
scope/provenance and acceptance treatment, or identify an existing permitted setter. Prefer a
separate fixture module so accepted capture/control/shader modules can stay byte-identical if
that packaging is feasible; no claim it is built, installed or approved. Implementation must
preserve host movement/camera behavior, log placement, run fresh L1, and verify the complete
placement-to-capture interval for respawn/possession, including buffered log events.

**Chat review REQUIRED** to resolve the mechanism/build conflict before the 24 legs or Stage 2.
**Chat review NOT REQUIRED** for this completed authorized preflight. Next mandatory checkpoint
is that ruling; a subsequent GREEN matrix still releases Stage 2 automatically. No new session
or context setting change recommended; same coherent workstream, no work in flight.

Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4** into Stage 3; a cost leg at that FPS needs an
explanation before reading the gate. High-water priors 17.5 MB StackOBot / 19.4 MB Lyra remain.
Stage 2 phase/event/teardown, Stage 3 oracle/cost and the prior coverage-limit block stay unrun
or limited exactly as previously reported. See the full external 081-07 matrix report and
081-07-evidence/postflight.json for final refs, unchanged artifacts and the restored boundary.
