# 094-31b — Chat ruling (pre-ruled from the live evidence at 22:40 IST): capture compiles on 4.25 with one fix; the 10-bit backbuffer is handled in the writer, not by changing the game. 094-32 closes C1 and builds C2 tonight

**Date:** 2026-10-31, about 22:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:** Codex's live messages and Code's midjob notes for 094-31b, from 20:59 to 22:21 IST. Code's review was still pending; 094-32 reads it at entry.

## Verdict
**The first real measurement of the capture port is good:**
- AnomalyCapture compiles on stock 4.25 with **one** fix: a missing `RenderUtils.h` include for `GPixelFormats`;
- Editor passed on attempt 2, and Game on attempt 1;
- FM1's lighter gate worked: two parallel actions, below-normal priority, no memory stop;
- the readback negative test passed: padded, undersized, null and 0×0 are rejected;
- the new 27-file manifest is bound, and the smoke passed.

**The first PIE capture exited normally but wrote 0 frames:**
- stock 4.25's backbuffer is 10-bit (`PF_A2B10G10R10`);
- the C1 writer accepts only 8-bit, so it correctly refused to write.

**Codex's workaround:** an 8-bit backbuffer through a command-line override.

**Then two sessions stopped during startup** on a guard race: the UBT guard couldn't re-read a short-lived `ValidatePlatforms` child's command line. Codex fixed it with bounded retries, plus a recorded command accepted only after the same process handle has exited. No crash, no upload, no protected change.

## Rulings

### FN1. Accepted: the build, the readback adapter's refusals, and the guard-race fix

### FN2. The writer handles the backbuffer the game actually uses (replaces the override)
**The formats:**
- **accepted as today:** `B8G8R8A8` and `R8G8B8A8`;
- **added:** `PF_A2B10G10R10`.
  - Cite 4.25's platform format mapping for its channel layout.
  - Convert each channel exactly to 8 bits with `v8 = (v10 * 255 + 511) / 1023`.
  - Alpha is not used.
- **any other format:** rejected and counted, never written.

**The rest:**
- **remove the 8-bit backbuffer override from every launch argv;**
- record the backbuffer format per session;
- **the test:** a synthetic 10-bit buffer with known values converts exactly, and an unsupported format is refused.

**Why:** capture must not change how the host game renders. Europa's backbuffer format is unknown, and the plugin has to work with whatever the game uses.

### FN3. C1 closes in 094-32
- **Rebuild under FM1,** then re-run both C1 sessions without the override: PIE on EA_Core, and `-game` on EA_Travel.
- **The gate is as FL6 sets it:**
  - 60 PNGs each at 1280×720 within 90 s;
  - an exact count, with no missing or duplicate index;
  - no black frame and no unknown format;
  - the correct check region;
  - the RHI, the dropped count and the backbuffer format recorded;
  - a normal exit.
- **094-31b's Game retry,** if it ran before 094-32, is kept as evidence. It used the override, so it doesn't close C1.

### FN4. C2, labels in sync, in the same job once C1 passes
- **Implement C2 per the plan** (`docs/europa/PHASEC-capture-port-plan.md` §C2) and FL2.
- **The session:** one session of up to 180 s, in both tick orders, with at least 3 apply-and-revert events per anomaly per order. It includes one block with capture pacing off (`IAI.Capture.Pace 0`).
- **The gate:** labels compared against pixels at every apply, revert and blink-toggle edge. **Zero mismatched frames,** and any ambiguous edge fails.
- **The negatives:** a separate one-frame label shift for apply edges, for revert edges, and for blink interior toggles. Each must fail on its own.

### FN5. The job and its window
- **FM1's build gate and FJ1's lean profile apply.**
- **If C1 fails,** stop before C2 and report.
- **The window for real sessions:** until 12:00 IST on 11-01.

## For the owner
- **Nothing to do.**
- **When the PC is free tonight:** the pending test runs first, then 094-32 runs by itself.
