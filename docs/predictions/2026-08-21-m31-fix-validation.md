# m31 FIX VALIDATION — pre-registered BEFORE any leg

**PRE-REGISTERED 2026-08-21, committed WITH the fix. Restate verbatim before any leg is read.**

The fix: the SVE wanted-handshake no longer compares two independent reads of `GFrameCounter`.
A plugin-owned monotonic serial is minted ONCE at the arm site, keys `PendingSnapshots`, rides
the backbuffer path's `ArmForCapture` and the SVE path's new pending-wanted FIFO, and is carried
by value end to end. The publish site consumes the oldest pending intent for the next ELIGIBLE
view family (scene/reflection-capture families are guarded out and counted). The ring key stays
`FSceneViewFamily::FrameNumber` — the one value measured-correct on the broken host.

## The legs

```
V-1  STACKOBOT PIE, same-seed structural pair vs banked m27-era behaviour — BOTH PATHS
     (SVE default AND IAI.Capture.SVE 0). Regression legs are TARGETED (G150: pool
     changes re-roll seeded auto-pool draws, so targeted blinking on StaticMeshActor_49,
     seed 777, is the comparable form). PASS: capture works, 90 frames, canonical gapped
     blink cadence, annotation.json FIELD SET unchanged, manifested 8/8-shaped events.
V-2  STACKOBOT PACKAGED (BenchGate, run_leg.ps1), one leg per path, same seed/target as
     V-1, PLUS a pre-fix leg on the outgoing staged exe for a true A/B. PASS: post-fix
     SVE leg structurally matches the pre-fix leg (event types, spans, counts, field
     set; A47 pose noise excepted); post-fix backbuffer leg matches too; the run-end
     gate line shows marksIssued = wantedMatches = submitsIssued = framesWritten = 90
     on the SVE leg; key ring missed=0 corrupted=0. (G76: PIE licenses mechanism only —
     the packaged legs are the certifying ones on this host.)
V-3  CONCORDE IN-EDITOR (owner runs, office instance drives): fixed/variable ON — the
     owner's NORMAL workflow — SVE 1: PNGs WRITTEN, total_frames > 0, the run-end line
     shows matches ~ marks. Currently zero. This is the before/after. THEN one
     backbuffer sanity capture (IAI.Capture.SVE 0) in the same session — the serial
     mint touched the only path that currently works on Concorde, so it is re-proven
     in the same visit (~30 seconds).
V-4  CONCORDE PACKAGED, one confirmation leg (both paths' artifacts read) before m31
     tags.
```

## V-3 FAILURE BRANCH, pre-registered

If PNGs are still zero on Concorde, the gate line and trace MUST localise the miss to ONE of:

- **arm-side** — `marksIssued=0`: CaptureCurrentFrame never armed (phase machine or SVE-path
  selection problem, not the handshake).
- **publish-side** — `marksIssued>0`, `wantedMatches=0`, `familiesIneligible` small: arms never
  met an eligible publish — the extension is not seeing eligible families at all.
- **eligibility-skip** — `wantedMatches=0` WITH `familiesIneligible` ≈ families seen: the guard
  is wrong for this host's main family (its views carry unexpected flags) — the guard, not the
  FIFO, is the defect.
- **pairing** — `wantedMatches ≈ marksIssued`, `submitsIssued ≈ marks`, `framesWritten=0` with
  per-frame pair-drop warnings (their token is named in journal 050): the ring or snapshot
  pairing lost the frames downstream.

**An unlocalised V-3 failure — a zero that the gate line cannot assign to one of those four —
is a GATE FAILURE OF THE INSTRUMENTATION ITSELF and is reported as such.**

## Interpretation constraints

- In-editor evidence licenses MECHANISM only (G76); m31 tags only after V-4.
- The StackOBot legs cannot show the Concorde defect (this rig never missed) — they exist to
  prove NO REGRESSION on the only locally testable host, on BOTH paths.
- `run_summary` field change, declared: `wanted_published` (shipped in `e3ab835`, never consumed
  by any tool, no banked artifact carries it) is RENAMED `wanted_matches` and counts publishes
  that consumed a pending arm — i.e. wanted-matches made. The annotation.json field set is
  untouched; P6 does not move.
