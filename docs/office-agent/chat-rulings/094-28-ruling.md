# 094-28 — Chat ruling (pre-ruled from the live evidence at 03:05 IST): the fixes hold, but our own UBT guard stopped the smoke session; 094-26b fixes the guard and the memory start line, re-arms and runs the authoring before morning

**Date:** 2026-10-30, about 03:10 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Live evidence:** Codex's messages and Claude Code's midjob notes for 094-28. Codex's report and Code's review were still being written. 094-26b reads both, and stops at entry if either shows a protected-state change, a crash or an upload.
- **The facts:**
  - The pre-check passed at 02:24 IST (14.88 GiB free commit, 9.30 GiB physical, GPU 0 %).
  - The smoke session started under the 300 s cap.
  - After 9.55 s the kit's FC6 guard stopped it on the editor's own UnrealBuildTool child, `-Mode=ValidatePlatforms -allplatforms`: "build action or unknown mode; only QueryTargets is permitted."
  - **Codex confirmed from 4.25 source that this is the editor's stock SDK-status check, not a build.**
  - No recipe slot was used, and no package was created.
  - Protected files, the bound binaries, the manifests and the build outputs are unchanged.
  - No upload was found in either log.
  - The EA8 exit proof passed. The docs commit is `dce872d`.

## Verdict
**FC2, FC4 and FC5 are accepted as built.**
- The launcher keeps its module alive, and the Python 2.7 harness passes.
- The analytics opt-out is source-proven, and no upload happened.
- The managed classes are in place.

**FC6 was implemented too narrowly,** and that is on the rule, not the code: I wrote it about build actions, and the guard became a list of allowed modes. The smoke session was stopped before the launcher ran, so the launcher fix is still unproven in the real editor.

**The memory start line also cost time:** the pre-check waited from about 01:00 to 02:24 on free commit (12 GiB line), while the PC's normal background load kept it at 10.8–11.4 GiB.

## Rulings

### FD1. Accepted
The 09428 arming is retired. FC2, FC4 and FC5 stand.

### FD2. FC6 is restated: judge the editor's UnrealBuildTool child by what it does, not by its argv
**Allowed:** any non-build mode the editor starts by itself (`QueryTargets`, `ValidatePlatforms` and the like). Its argv is still recorded.

**A STOP, any of these:**
- a build invocation: `-Mode=Build`, or a target, platform and configuration triple;
- a compiler, linker or UHT child;
- a write to build outputs (`Binaries`, `Intermediate` build products, makefiles);
- a change to a bound binary.

**Can-fails:**
- `ValidatePlatforms` and `QueryTargets` pass;
- a build triple, a compiler child, and a write to `Binaries` each stop.

### FD3. The memory lines match this PC (amends EY3 and EX2)
- **Free commit at the start gate:** 8 GiB (was 12).
- **The in-session commit stop:** 4 GiB (was 6).
- **Physical memory:** 6 GiB at the start, unchanged.
- **Why:**
  - this PC's normal background load (the Claude app, the Mailbox dashboard, the Epic launcher, Steam) keeps free commit around 10–11 GiB;
  - the page file is system-managed on D:, which has about 70 GiB free, so commit can still grow under load;
  - the old line held the start for about 90 minutes tonight.

### FD4. 094-26b, now: fix, re-arm, then the real authoring run, in one job
**In order:**
1. **FD2 and FD3,** with their tests and can-fails, and the Python 2.7 harness for anything changed.
2. **A stand-in rehearsal of the changed checks only.**
3. **Re-arm for 09426b:**
   - fresh slots for the smoke session, EA_Core and EA_Travel;
   - EW2's scope otherwise;
   - the 09428 and older armings refused.
4. **One live, observe-only gate reading.**
5. **The real run, under FA5's idle rule.**
   - **The window:** until 07:30 IST on 10-30. A session already started may finish.
   - **The order:** the smoke session (FC3), then EA_Core, then EA_Travel.
6. **Evidence** per EY7 and FC3–FC5, and a lean exit (FA6, a milestone).

### FD5. No dry runs in 094-26b (this re-arm only)
- The 09428 dry runs passed on the same kit.
- FD2 and FD3 are small, and their tests and the stand-in rehearsal cover them.
- The kit's own preflight re-checks protected files, Content, binaries and routing before every real session.

### FD6. 094-27 waits behind 094-26b
- It reads 094-26b's results instead of 094-28's.
- FD2 and FD3 carry into its kit.

## For the owner
Nothing to do. The run happens while you sleep, if the PC is idle. If it's still going when you wake up, don't touch an open Unreal window.
