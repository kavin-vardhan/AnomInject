# 094-27c — Chat ruling (pre-ruled from the live evidence at 22:20 IST): the camera fix worked, lighting_mismatch passed with pictures, and the owner's directive cuts every check that isn't about the port (the lean profile). 094-27d finishes A7 tonight

**Date:** 2026-10-30, about 22:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's live messages and Code's midjob notes for 094-27c,** to 22:15 IST, with 14 of 18 runtime sessions done.
- **The owner's directive (22:20 IST):** get the job done fast; no unnecessary long checks or tests that aren't directly relevant to the porting.
- **Pending:** Code's review was not yet written. 094-27d reads it, and stops at entry only for a protected-state change or an upload.

## Verdict
**The results so far:**
- **A6 re-authoring:** both maps PASS, and the camera reads back pitch −12, yaw 0, roll 0. FI2 worked.
- **Smoke and A3 Load:** PASS. The Load leg did not crash, so FI3's re-entry guard held.
- **A7 lighting_mismatch:** **PASS, with pictures.** It is the first anomaly proven on 4.25.
- **A7 lod_corruption:**
  - 6 applies and 6 reverts, with no invalid windows;
  - but 0 events passed the picture check ("No visible active picture").
- **A7 missing_object:**
  - 6 applies and 6 reverts;
  - but all 6 events failed the clean-frame noise check.
- **A7 blinking:** a controlled stop. Access was denied while reading the live probe file (a sharing violation).
- **Controls:**
  - LOD NoApply and missing-object NoApply: PASS;
  - blinking NoApply, lighting NoApply and blinking SkipRestore: invalid, from missing monitoring coverage (the GPU reading was often unavailable), and for SkipRestore also clean-frame noise.
- **A5 WorldsPIE:** FAIL. The driver saw no three two-world cycles, and there was no `probe_stop`.
- **A5 WorldsGame:** FAIL. The kit stopped it at FD3's 4 GiB commit line, under the owner's render load.
- **A6 GC:** **crash #1.** Our test probe calls `NewObject<UObject>` (`GCProbe()` line 437), and 4.25 rejects an abstract class in `-game`. That is test code, not the plugin. FI4 let the run continue.

## Rulings

### FJ1. The lean profile (the owner's directive), from 094-27d on
**Dropped until the D-lite push:**
- the full EA8 entry and exit proofs (the 104 GB P9 v2 re-hash);
- the canary scans, the EK1 scan and the fresh canary per job;
- the EU1 command audit;
- the ledger's charged-growth accounting. The drive free-space lines stay.

**Dropped unless the launcher or the kit's core changes:**
- stand-in rehearsals;
- dry-run sweeps of every leg;
- live observe-only gate readings;
- the smoke session.

**Tests:**
- only for the code that changed: its unit tests, the Python 2.7 harness for changed in-editor scripts, and the keyword-`Rotator` static check;
- one negative test per fix, and nothing more.

**The gate:**
- **one reading;**
- **it waits only on:**
  - FD3's memory lines;
  - the drive lines, with D: now 25 GiB to start and a 15 GiB floor;
  - 5 minutes of idle time;
- **record only:** GPU load, human input and monitoring coverage. They never block a start, and they never invalidate an event;
- **a picture event counts** when its before, during and after screenshots exist and pass the picture check.

**Deferred out of Phase A (FUTURE, revisited after D-lite):**
- A5's worlds and lifecycle legs (two-world PIE, `-game`, travel);
- A6's reflected-GC leg, and the probe rebuild it needs;
- A8's admission and seeded-auto checks;
- all NoApply and SkipRestore controls.

**Stays, because it is cheap and protects the owner:**
- `m51` and the protected refs, by read-only git, at the start and the end;
- the Paris host's Content, Config, descriptors and `.uproject`, hashed before and after each session. A change is a STOP;
- EU4's and FC4's upload switches on every argv, and a per-session log check for upload signs. A sign is a STOP;
- the session rules:
  - one session per leg;
  - the session limit;
  - the owned-tree stop by PID, with FH5's monitor adoption;
- FI4's crash rule;
- no push and no tags; codenames only; never touch the owner's programs or settings.

**Code's review is short:**
- the anomaly results, with its own event count and one look at each contact sheet;
- any crash, upload sign or protected change;
- the diff of the fixes.

There is no rulings table, and no citation re-checks except for new 4.25 claims.

**Binding documents:** a request binds its own ruling, this profile, and only the rulings it names. Nobody re-reads the whole chain at bootstrap.

### FJ2. Phase A's exit is redefined
**Phase A closes when:**
- A3 Load has passed (done), and
- each of A7's four anomalies has at least 3 events that visibly apply and revert, by the picture check or by the owner's eye check of the contact sheets.

**Then D-lite follows straight away:** the owner pushes the branch, and we run an office compile and injection smoke on Europa's fork.

### FJ3. 094-27d: finish A7 tonight, in one job
1. **Entry:** FJ1's fast checks, plus 094-27c's results (stop only for a protected change or an upload).
2. **Diagnose from the kept 094-27c pictures and probe rows,** with no new session:
   - **lod_corruption:** why no change is visible. Check the forced LOD value (1-based), LOD1's difference at this camera, and the picture region;
   - **missing_object and blinking SkipRestore:** what changes between clean frames. Compare global brightness drift (auto exposure) with local change;
   - **blinking:** the sharing violation on the live probe read.
3. **Fix, in this order of preference:**
   - **a checker or region was wrong:** fix it, then re-read the kept evidence (FH4). There is no new session for that anomaly if the re-read passes;
   - **the live probe read:** it retries on a sharing violation;
   - **brightness drift:** lock exposure for picture sessions with a command-line override proven from 4.25 source. No config edit (like FE1);
   - **no visible LOD change in the pictures:** fix the anomaly's parameters or target in the driver. No re-authoring.

   Test only the changed code (FJ1).
4. **Re-arm only the A7 positive legs that still fail,** with fresh slots, and run them when the PC is idle (FJ1's gate).
5. **Contact sheets for all four anomalies,** with one index for the owner: lighting's from 094-27c, plus the new ones.
6. **Exit:** FJ1's fast checks, a commit, and the report.

**If a failure can't be explained or fixed without re-authoring,** report it with its pictures as NEEDS-DECISION, and still finish the others.

**The window:** until 07:00 IST on 10-31.

### FJ4. Folded in
- **Code's O1:** FD2's build guard needs no live exercise.
- **Code's O4:** remove the three comment lines in `tools/europa` (the plugin's no-comments convention) in this job's change.
- **A standing rule (from 094-27b's G1):** every driver step runs once, behind a guard, and the Python 2.7 harness models a load that takes several ticks.

## For the owner
- **Nothing to do.**
- **Tonight:** when you're done, just leave the PC. The remaining tests start after 5 idle minutes.
- **Tomorrow morning:** a 2-minute eye check of 4 picture sheets. Chat will say which files to open.
