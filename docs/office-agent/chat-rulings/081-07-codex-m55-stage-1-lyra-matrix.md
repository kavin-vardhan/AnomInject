# 081-07 — m55 Stage 1 Lyra matrix: L2 mechanism preflight
VERDICT: NEEDS-DECISION

2026-09-21. Authority: [Chat ruling 081-06](D:/IntrusiveAnomalies/_reviews/081-06-chat-ruling-lyra-placement.md).

## 1. Disposition

**The permitted placement mechanism is absent from the accepted candidate. Adding the bench
console lever requires a rebuild, which the same ruling expressly forbids.** No new qualification
attempt was started. This is a constraint conflict discovered before execution, not a product FAIL.
L2 can be implemented without Lyra host source changes; the blocker is the unchanged-binary rule.

**36/60 legs remain accepted; 24 remain UNRUN.** Stage 2's automatic GREEN condition is unmet.
Every remaining leg retains all three fresh L2 attempts. The prior three G1-native L1-invalid
attempts and the original failed-precondition-undefined G14-synthetic attempt remain preserved.

**Chat review REQUIRED** to resolve the mechanism/build constraint before running the 24 legs or
starting Stage 2. **Chat review NOT REQUIRED** for this completed, authorized source preflight and
documentation. The next mandatory checkpoint is Chat's mechanism/build ruling. A later GREEN
matrix still releases Stage 2 automatically under 081-03, absent a new finding.

## 2. Mechanism inspection — exact accepted source

Inspected implementation `3dc1ddaa355093f05610c8eac94d7cf15c48c7f0`; its Source/Shaders are identical
to feature entry `4662e6ffd498b0ec789330789dbf8dde242a8a65`. Frozen complete plugin and CaptureBench
Source trees (112 files), hashes, command-text inventory and setter search hits are in
[mechanism-inspection.json](D:/IntrusiveAnomalies/_reviews/081-07-evidence/mechanism-inspection.json)
and [source manifest](D:/IntrusiveAnomalies/_reviews/081-07-frozen/manifest.json).

| Route | Source finding | Consequence |
|---|---|---|
| (a) Existing control server | Dispatch recognizes snapshot subscription, injection, configuration and capture requests, but no pose setter or generic console execution. Unknown types fall through to `SendAck`. | An invented `set_view` request could be acknowledged without changing anything. No such request was sent. |
| (b) Existing plugin bench commands | `TeleportTargetOffscreenAt` moves live anomaly targets by +1,000,000 cm Z during capture; `MaskPairingProbe` moves its own probe. `Letterbox` changes camera aspect. None places the pawn/view. | Existing commands cannot implement L2. Adding a C++ command changes a compiled module. |
| Existing CaptureBench fixture source | No pawn/view placement setter found. | A new fixture command there likewise needs compilation and a changed fixture binary. |
| Engine `BugItGo` | Calls `Ghost()` before and after pawn teleport, then sets control rotation. Pawn position is not the recorded camera origin. | Not the stipulated fixture-only lever and introduces extra movement behavior. Not used as a substitute. |

Reviewable source anchors:

- [Control-server dispatch](D:/IntrusiveAnomalies/_reviews/081-07-frozen/AnomalyInjector/Source/AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp:343), including its final fall-through acknowledgement at line 715.
- [Target-teleport command](D:/IntrusiveAnomalies/_reviews/081-07-frozen/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:7040); [probe position update](D:/IntrusiveAnomalies/_reviews/081-07-frozen/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:1815).
- [Letterbox implementation](D:/IntrusiveAnomalies/_reviews/081-07-frozen/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureLetterbox.cpp:1); [capture compile gate](D:/IntrusiveAnomalies/_reviews/081-07-frozen/AnomalyInjector/Source/AnomalyCapture/AnomalyCapture.Build.cs:340).
- [Installed-engine BugItGo excerpt](D:/IntrusiveAnomalies/_reviews/081-07-evidence/engine-BugItGo-excerpt.txt), original CheatManager.cpp lines 1036–1093; original file hash recorded in inspection JSON.

These are source findings, not claims from a launched L2 fixture. No live pose-setting experiment,
engine cheat, host edit, binary patch, rebuild or cook was used to bypass the constraint.

## 3. L2 recorded; not implemented or exercised

G275 and the active prediction amendment record the ruled sequence: after 45 seconds settle and
the last logged host respawn, place once at camera reference (-443.98,-70,212.00), pitch/yaw/roll
(0,0,-0.08); independently require fresh L1 within 150 cm / 10 degrees; reject a logged
respawn/possession between placement and capture_start. In-leg motion remains allowed. Placement
cannot certify its own result. StackOBot B1 is not portable to this Lyra fixture.

Each of G1–G12 native/synthetic has three fresh total attempts shared with the handled-ensure
exception. Passing L1 followed by an ordinary event-floor failure is a real FAIL, not retryable.
G11 retains its previously approved 9-frame lifetime-only exception. The accepted G14-synthetic
qualification remains accepted. No thresholds, host rendering defaults or existing verdicts changed.

Proposed concrete resolution for Chat: authorize a fixture-only placement lever build with an
explicit binary scope/provenance rule. Prefer a separate fixture module if feasible so the accepted
capture/control/shader module set stays byte-identical. Alternatively, identify an existing permitted
setter that satisfies L2. Neither option is implemented or assumed approved here. A new lever must
preserve host movement/camera behavior, distinguish camera origin from pawn origin, log placement,
retain independent L1, and validate the complete placement-to-capture log interval, including
buffered respawn/possession events. Acceptance treatment for any changed binary must be explicit;
the no-rebuild instruction was not silently weakened.

## 4. Matrix and unchanged carry-forwards

| Fixture / gates | Accepted | New L2 attempts | Remaining |
|---|---:|---:|---:|
| StackOBot G1–G15, both orders | 30 | 0 | 0 |
| Lyra G13–G15, both orders | 6 | 0 | 0 |
| Lyra G1–G12, both orders | 0 | 0 | 24 |
| Total | 36 | 0 | 24 |

[Current ledger](D:/IntrusiveAnomalies/_reviews/081-07-evidence/final-ledger.json) links the byte-preserved
prior ledger and attempt history. All three old G1-native attempts remain INVALID-FIXTURE, with
no capture sent. This preflight consumes no attempt. Prior evidence/report files remain unchanged.

Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4** into Stage 3; any cost leg at that FPS needs an
explanation before its gate can be read. High-water priors remain 17.5 MB StackOBot / 19.4 MB Lyra.
MainWorld strict invariants and CB occupancy-plus-own-mask_value acceptance stand; EXTRAS remains
accepted timing/float noise. [Entire previous coverage-limit block](D:/IntrusiveAnomalies/_reviews/081-07-evidence/prior-coverage-limits-verbatim.md)
is copied byte-for-byte, with no expanded coverage claim. Stage 2 phase/event/teardown and Stage 3
oracle/cost remain unrun. Documentation completion does not make the matrix or release GREEN.

## 5. Changes and final boundary

Documentation-only feature commit `92a52ab` is pushed: G275, active prediction amendment, session
journal and current CLAUDE.md status. No source, shader, verifier, harness, host or fixture behavior
was changed. Frozen final documents accompany the source manifest.

Verified final state in [postflight.json](D:/IntrusiveAnomalies/_reviews/081-07-evidence/postflight.json):

- Main restored to `m51` / `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`; tracked clean, 11 prior handoffs preserved.
- Lyra remains detached `caa68c6799db023f7a24c0d6cfc56124ce27b542`; original nine module/PDB/manifest files unchanged.
- CaptureBench `b2ca7646e094b61d423f879dbad47af116f0ae10` unchanged, eight prior untracked files preserved.
- Master local/origin remains `031a103b18b90b2ce9dd626b22e9498717c60815`; only m55 feature local/origin refs advanced.
- Game candidate SHA-256 `aebd09ea195b16317ad2d4c09ad621facfa12f0588636d9f5400f2331bf44d58` verified built/staged/archived. All nine archived Lyra candidate files match the accepted manifest. Candidate modules were not installed this round.
- Five containers, archived predecessor 0844220E, prior reports and evidence/frozen trees preserved; CURRENT history retained byte-for-byte. Git diff checks passed.
- No engine/game/build process, watcher or owned work remains in flight. No merge, tag, mailbox brief, owner-app or office-data action.

Same coherent workstream; no fresh-session or context-setting change recommended.

VERDICT: NEEDS-DECISION
