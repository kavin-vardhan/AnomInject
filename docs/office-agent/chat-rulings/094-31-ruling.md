# 094-31 — Chat ruling (interim, 18:05 IST): C1's source is ready, but the armed build gate (16 GiB of free commit) can never pass on this PC. Restart with a lighter incremental gate (094-31b)

**Date:** 2026-10-31, about 18:05 IST · **Author:** chat Claude (orchestrator)

**Inputs:** Codex's live messages and Code's midjob notes for 094-31, from 10:33 to 17:57 IST.

## Verdict
**Ready:**
- Codex implemented and committed C1's source: capture compiled in on 4.25, the readback adapter (FL3), and the tests;
- it also tightened the arming check, so a stale build can never be bound.

**Blocked:** the first build never started. It waits on the A3–A5 build memory gate, which needs **16 GiB of free commit**.
- **Free commit seen on this PC:**
  - the highest in two days was about 14.9 GiB (10-30, 07:02), with nothing of the owner's running;
  - today it ran 0.5–12 GiB, under the owner's python and ffmpeg jobs;
  - there is a standing load of about 12 GB besides: `vmmem` 4 GB, `wlanext` 3.6 GB, the Epic launcher 2.3 GB, and our claude processes about 2.7 GB.
- **So the gate is unpassable.** This is my miss: C1's request kept the old gate, and that gate was sized for full builds.

## Rulings

### FM1. A lighter gate for incremental 4.25 builds (amends the build memory gate for these jobs)
- **UnrealBuildTool runs with `-MaxParallelActions=2`** at below-normal priority.
- **Start:** one reading of at least 8 GiB of free commit.
- **In-build stop:** if free commit falls below 3 GiB, stop our owned build tree by PID, then retry when the start line holds again. At most 3 attempts per target.
- **Why:** an incremental build of one or two modules with two compiles at a time needs about 3–5 GB, and the owner's own jobs are always protected by the in-build stop.

### FM2. 094-31b: finish C1 from the committed source, under FM1
- **Entry:**
  - if 094-31's Codex process, or any of its kit processes, is still alive, stop it by the PIDs recorded in `code-receipts-094-31\` (never by name);
  - then FJ1's fast checks.
- **The rest of 094-31's order is unchanged:** the build of both targets, the first-build error counts per module, the fixes, re-binding the manifest, the negative test, and the two C1 sessions.
- **The window:** until 07:00 IST on 11-01.

## For the owner
- **Close the watcher window, then double-click `start_watcher.bat` again.**
  - That ends the stuck job. Its work is saved.
  - The restarted watcher picks up 094-31b by itself.
