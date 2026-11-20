# 081-10 — m55 fixture fix and stopped preflight validation

2026-09-22. Authority:081-09-chat-ruling-fixture-fix.md. Codex implements; Chat orchestrates;
Code independently reviews after Stage3. This is the explicit fix/preflight report boundary.

## Disposition

**NEEDS-DECISION.** Implemented the bench input lock and extended one-shot placement to the Game
target/StackOBot. All three Development builds passed. The **first enabled preflight refused at
the input-lock readiness check**, before settle, placement or snapshots. Required5/5 was not
achieved:0 qualified,1 attempted,4 enabled and3 disabled launches UNRUN.

I followed the ruling's stop condition. **No retry, capture or fresh qualification allowance.**
This is an implementation/readiness failure, not a new Stage2 measurement result. My guard
requires a possessed pawn even though locking input only needs a controller. The map log also
says No Spawnpad found; the combined refusal does not directly identify the missing pointer.

**Chat review REQUIRED** for this failed validation disposition and correction/revalidation before
081-11. **Chat review NOT REQUIRED** for completed authorized implementation/build and boundary
documentation. Next mandatory checkpoint is Chat's ruling on081-10. Stage3 remains held.

## Implemented source and build identity

Built source: `4a1475670bca25d3e6735a137547637bb986482a`.
Game SHA256: `b283e34c13a8229680848e6b87ac5e448b457a2e0f0e5ae8a15759742d0384da`.
Size241,723,392 bytes. Built, staged and archived copies agree. Archive:
`D:/IntrusiveAnomalies/_binary_baselines/StackOBot.exe.m55-stage2-B283E34C`.
Lyra11-file candidate archive:
`D:/IntrusiveAnomalies/_binary_baselines/m55-stage2-fixture-lyra-23258325`.
Full module/container hashes and A44:081-10-evidence/binary-v1.json.

Only three implementation files changed: descriptor, AnomalyBenchModule.cpp and the typed
control-server bridge. No measurement arithmetic, label writer, observable rule, shader, content,
auto/selector pool or verifier changes. No host source changes or cook.

- AnomalyBench now allows Game/Editor and excludes Client/Server and Shipping. Commands remain
  explicit, fixture-flag/map gated and unavailable during active capture.
- IAI.Bench.InputLock1 calls SetIgnoreLookInput(true)/SetIgnoreMoveInput(true) once. Repeated enable
  does not stack. Release decrements only its owned counts, preserving pre-existing host counts;
  implemented release paths are explicit off, observed run end, world cleanup, module shutdown,
  lost controller/subsystem. It does not reset host input flags or pin the view. **All enable/release
  runtime behaviour remains unqualified because this first enable refused.**
- StackOBot desired camera is(-1500,0,260), rotation(0,0,0), cited from081-09 R1V3_A1 snapshots
  and the B1 CALIB_BBOX(0,485.2,306.1,234.8). Placement uses measured camera-to-pawn offset and
  requires the CB input lock. Lyra's reference is unchanged. No replacement camera, ghost mode,
  velocity reset or persistent pose pin. **Placement was never attempted.**
- Typed bench_input_lock/bench_place_view requests explicitly load the linked fixture module.
  Existing cooked descriptors predate it; explicit loading avoids recooking. Module loading
  was exercised successfully enough to emit the fixture command's refusal. An executed receipt
  only means the command was called; the observer separately requires success logs and fresh B1.

StackOBot Game, StackOBotEditor and LyraEditor Development all exited0. A44 found UTF-16 input-lock
and placement literals plus existing measurement controls; the invented token was absent in both
encodings. Pre-fix2C21EDE5 remains archived, unchanged, as do AEBD09EA/0844220E and all five cooked
containers. There was one source build revision and one attempted validation launch.

## Predeclared validation and actual result

Predictions were committed before implementation/build in the081-10 journal. Intended order:
ON1,OFF1,ON2,OFF2,ON3,OFF3,ON4,ON5. Same CB map,1280x720,AA0/AE-off and diagnostic snapshot recipe
as081-09. ON would lock before five-second settle, place once, wait700ms and collect90 snapshots.
Strict ON gate: settle sample0, modal rotation0/0/0,100% coverage, B1 PASS, calibrated origin,
exactly one successful lock/placement and explicit unlock restoring prior ignore flags.
Foreground PID/sample counts are diagnostic here, including0/90 passes as the new ruling permits.

Actual `F12_ON1`:

| Item | Reading |
|---|---|
| Owned process | PID35828, stopped/joined by runner finally |
| Game/world readiness | 3.3320459 seconds after launch |
| Authentication | Succeeded at14:39:15.489 UTC |
| Fixture command | REFUSED at14:39:15.528 UTC, GFrameCounter43 |
| Log | `IAI-INPUT-LOCK REFUSED fixture/map/ready-controller/inactive-capture/argument gate` |
| Input lock ON | Absent |
| Settle/placement | Never reached |
| Pose/foreground sample count |0; no snapshot file |
| B1/coverage/pose verdict | Unavailable, not pass |
| capture_start/injection | Not sent |
| Runner exit/status |1 / PROTOCOL-FAIL (`Input lock not observed`) |

The wrapper calls this PROTOCOL-FAIL, but the substantive result is an explicit fixture guard
refusal. No measurement or5/5 certificate can be inferred. All remaining seven launches are UNRUN;
I did not repeat ON1 or quietly reset validation. Evidence:runner-F12_ON1.log, F12_ON1/game.log,
launch.json, validation-results.json and final-ledger.json under081-10-evidence.

## Bounded cause read and concrete correction needed

InputLock currently checks `!PC || !PC->GetPawn()` before calling the controller input methods.
The latter condition is an unnecessary restriction introduced by me. Engine Controller.cpp
confirms the ignore flags are controller counters; they do not require a possessed pawn.
The same map startup logs `No Spawnpad found!` and associated host Blueprint warnings. The
composite refusal line does not distinguish absent controller from absent pawn, so pawn absence
is a supported hypothesis, not a measured fact from this launch.

PlaceView also uses GetPawn. PlayerController.h exposes GetPawnOrSpectator separately. A corrected
readiness path should log controller, pawn, spectator and active view-target identities explicitly,
enable input locking when the controller is ready, and resolve the actual camera-owning actor for
offset placement. Do not spawn a substitute camera/pawn or modify host source to make this pass.
The branch needs this correction before any claim that F1/F2 work on CB. No post-failure edit to
implementation, rebuild, diagnostic launch or revalidation was made after the required stop.

This does not reopen Chat's accepted focus-transition explanation or call for another old-binary
A/B diagnosis. It identifies a separate assumption in my attempted correction. G278 records the
requested look/move lock + placement rule and keeps measurement focus requirements explicit.

## Gates and authorization carried forward

No fresh three-attempt allowance has been released. Old DELAY_N's three invalid attempts remain
uncredited/exhausted. The four older static twin legs remain accepted; they must still be rerun
on a successfully validated replacement binary. Stage1 stays ACCEPTED on36; the other24 Lyra
legs are **DROPPED-BY-OWNER-DECISION**, with historical invalid evidence retained.

081-11 R2–R4 remains pending: true cooked-mesh occluder, natural moving twins, all other phase/
hide/delay/nohold/lifetime gates, mandatory actual teardown and six-leg AEBD09EA legacy invariance.
F3 requires foreground throughout each future measurement leg and a PID/sample-count audit.
This round exercised no measurement runner; its integration/audit remains required for081-11.
081-12 R5 Lyra stays last, scoped null/solid natural +G3/G270; zero runtime attempts used and the
first-leg three-attempt auto-drop rule unchanged. No Lyra Stage3/cost runs. Stage3 release requires
GREEN R2–R4 plus R5 resolved.

Carry **FPS 21.5 vs 30 on evidence-OFF MAIN4**, the prior verbatim limits and open teardown limit.
No cost, arithmetic-oracle, release, merge or shipping conclusion is made by this report.

## Restored boundary and read order

Final feature documentation commit/push, frozen source/docs and all hashes are recorded in
081-10-evidence/postflight.json and081-10-frozen/manifest.json. Main source restored m51
`53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`; Lyra detached source restored
`caa68c6799db023f7a24c0d6cfc56124ce27b542`, its nine entry modules restored and only the two newly
introduced AnomalyBench module files removed. Built/staged Game remainsB283E34C and main
StackOBotEditor modules remain rebuilt candidate binaries; restored source is not binary identity.
Master/m51/CaptureBench and pre-existing untracked files remain unchanged. No merge/tag/cook,
office transfer, owner-app modification or mailbox brief. No owned process/build/watcher remains.
All CURRENT bytes, including newer unrelated Blender/video boundaries, are preserved underneath
the plugin handoff. No fresh-session switch recommended for this continuing plugin workstream.

Read this report,081-09 ruling, frozen081-10 journal, final-ledger/bounded-cause-read, the first
launch's full logs/metadata, binary-v1/build logs, frozen manifest and postflight. Prior accepted
measurement evidence is still the081-08 report/banks; prior diagnosis remains preserved081-09.

## Questions for chat

The prescribed first ON validation refused before pose sampling, so I stopped under the <5/5
rule. Please rule on correction/revalidation: remove the unnecessary pawn prerequisite from F1,
add explicit readiness identities, resolve the real pawn/spectator/view owner for F2 within the
existing bench-only/no-host/no-cook limits, and rerun the prescribed5 ON/3 OFF validation on a
new archived build. Retain F12_ON1 uncredited and renew no measurement allowance until5/5.

VERDICT: NEEDS-DECISION
