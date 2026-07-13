# 021 — m15: content-clock default reverted to wall (RESOLVED) (2026-07-13)

## Goal
Flip the content-clock **default back to `wall`** and correct the m14 docs from "unresolved / client
FAST-risk open" to the settled state. Small, focused: one-line default + doc correction + commit/tag/push.

## Why (the resolution — this CLOSES journal 020's open item)
m14 shipped the default as `game` on an owner override, pending validation, with the client titles'
clock behavior marked UNRESOLVED (mixed-clock hypothesis: they showed a FAST signature at ratio ≈ 2 and
a SLOW signature at `Fps` 120/240). The owner has now **tested wall vs game on the actual office
machine** (Until Dawn, Concorde):

- **WALL produces correct-SPEED videos** for those titles. The video LENGTH varies with the real capture
  time — which is CORRECT for wall-clock content (natural playback SPEED is the criterion, and wall
  passes it). So **the client titles are wall-clock.**
- The earlier `Fps` 120/240 "slow motion" was an **extreme-forced-ratio artifact**, not game-clock
  evidence.
- **StackOBot is game-clock** (unchanged); it sets `game` via its own build's ini.

Therefore the shipped default returns to **wall**, which is client-safe: a `game` default would stamp
the client's wall-clock videos at target on a slow run and play them ~`speed_ratio`× (≈2×) FAST — the
Issue-2 regression. The mixed-clock question and the "a client build must re-evaluate per title" open
item from journal 020 are **CLOSED**: the client titles are wall-clock, the wall default is correct for
them out of the box, no per-title action is needed for a client build.

## What changed
- **Code (one line):** `EContentClock ContentClock = EContentClock::Game → Wall`
  (AnomalyCaptureSubsystem.h). The GConfig-absent fallback keeps the member default, so an unset
  `[AnomalyCapture] ContentClockDefault` now resolves to `wall` again. The ini key + the
  `IAI.Capture.ContentClock` console command still accept both tokens unchanged — ONLY the unset-default
  flips.
- **Docs corrected to the RESOLVED state:** `capture-fps.md` (content-clock section + knob),
  `client-delivery.md`, `gotchas.md` G70 — all now say default=wall, the client titles are wall-clock
  (owner-tested), the "do NOT flip to game for a client build (Issue-2 2×-fast regression)" warning as
  settled fact, the per-build ini mechanism, and the wall-clock video-LENGTH-varies-but-SPEED-natural
  property. CLAUDE.md status → m15.

Nothing else in m14 changed: game/wall stamp branches, warnings, the setting, the run_summary
`content_clock` field, fixed timestep / pacing / labeling / ground-truth — all as gated in m14.

## Re-verify (full editor-closed rebuild + relaunch on the new binary)
Fresh session with NO ini key → Initialize logged "AnomalyCapture subsystem initialized … Content clock:
wall (stamp sustained on slow runs)" and Status showed `clock=wall` — the inverse of the m14 post-flip
game default, confirming the revert.
- RESULT: **PASS** — after the editor-closed rebuild + relaunch, a fresh session with no ini key logged
  "Content clock: wall (stamp sustained on slow runs)" (Initialize) and Status showed `clock=wall`.

## State / Hand-off
- Plugin: m14 `debb01b` → m15 (this commit), tagged `m15`. Base was clean m14. Dashboard untouched
  (m13 `958451d` + the m14-turn host-tools chore `8958ed7`).
- The content-clock feature is now settled: **wall default (client wall-clock titles), game via ini for
  StackOBot.** No open items remain on the clock question.
