# REPORT for 094-20-europa-probe-fix-rebuilds-rerun.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-20-europa-probe-fix-rebuilds-rerun.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-27 14:38:15 | finished: 2026-10-28 01:05:15 | duration: 627 min | exit code: 0
context at last turn: 657993 tokens (66% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-20-europa-probe-fix-rebuilds-rerun.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner:**
- The disk-pause tooling and the probe crash fix are written, committed (`1722348`, local only) and checked on paper. None of the six rebuilds ran: the PC never had enough free memory, and D: kept dropping by about 22 GiB for reasons we couldn't identify.
- Nothing protected changed, nothing was launched, and no secret or canary leaked.
- Chat needs to rule on one small rule breach, on the texture finding, and on how to fit the rebuilds onto this machine.

**Start state (14:40–15:37 IST)**
- **`m51`** matched P9 (`53bf725`, index `526aba7f`, 0 tracked changes, 15 untracked) at start, at both pre-launch checks and after exit.
- **Protected state** passed. P9 v2 file `8E3ED095…` unchanged. Bound manifests 27/39/44 re-hashed clean.
- **31124 marker** in place. The launcher is still the old editor-spawned instance (pid 26116, running since 10:38), not a clean restart.
- **Owner-slot temp files:** my first reads failed, but they open fine with delete sharing.
- **EK1 pre-scan:** 41,320 files, 0 secret values.
- **Free space and memory:** D: was 70.98 GiB at 14:40, then dropped to 54.45 GiB at 15:20 while a third owner Blender ran. I held the launch until D: read at least 70 GiB on two polls; it went at 15:37. Commit free memory was 6–13 GiB against the 16 GiB the builds need.

**The Codex job**
- Run folder `2026-10-16-094-01-europa-phase-a-plan`, thread `01a14334`.
- 15:37:13 → 00:46:32, 32,959 s, exit 0, 191 commands. Ledger row `…-resume18` written once.
- 17 disk pauses, about 2.6 h in total; 16 resumed and the last timed out at the 9-hour limit.
- Its verdict: `NEEDS-DECISION | Nine-hour limit during a foreign disk pause; probe fix committed but uncompiled. Six builds, EI4, rebinding, EU5 and full EK1 remain pending; an EU2 pause-write violation is disclosed.`

**EU2, the fix and the rebuilds**
- **EU2 works as ruled:**
  - a drop below the floor stops only our own process tree and pauses;
  - it resumes after three readings above the 55 GiB warning line, a minute apart;
  - after 4 hours with no recovery it ends "no disk window";
  - a drop our own writes would cause is still a hard stop.
- **EU2 tests:** Codex's self-tests pass 12/12 when I re-run them. I also drove the real supervisor through four cases: a foreign drop pauses then resumes, a port-caused drop stops, no recovery hits the 4 h timeout, and a drop during a "build" stops only that tree while my control process survives. A mutant that ignores the port's own growth is caught.
- **All 17 real pauses were correctly judged foreign:** the port's own growth on D: was at most 139 MiB each time. My minute-by-minute readings show D: below 50 GiB inside 13 of the 17 pause windows; the other 4 dips were shorter than my sampling.
- **The fix:**
  - the actor iterator is replaced with guarded per-level actor lists;
  - both `FindObject` lookups are moved to world initialization behind exit and GC guards, kept as weak pointers;
  - cleanup at exit now writes a "skipped" row with its reason.
- **Fix checks:**
  - the fix is identical in all three hosts and uses `ENGINE_MAJOR_VERSION` for engine differences;
  - there is no production source or content edit;
  - Codex's static check passes, and its 9 mutants are caught in every host and the templates.
- **My own mutants:** all 5 in the ruled families are caught, including a `TActorRange` in `Stop()` and a `StaticFindObject` in `Cleaned()`.
- **Rebuilds: 0 of 6, so no EI4 and no re-binding.** The manifests stay bound to the old binaries. The only change in the build trees is the 4.25 Editor makefile, deleted by receipt; its hash matches my before-snapshot.

**EU5 and the command audit**
- **EU5:** Codex did not start it. My own read-only header scan finds exactly one multi-tile texture among 698: `T_Vetro02_N`. The owner-leg log builds it five lines before the ensure fires. It is used by the glass master material `MM_Glass` and six glass instances.
- **Command audit:** my independent audit agrees with Codex's. Of 191 commands, 4 were direct read-only (the first four, before the filter wrapper existed), 187 went through the wrapper, and 0 were direct write-capable.

**My review** (`_reviews\094-20-code-review.md`): 25 OK, 9 N/A, 2 GAP rows, 4 OPEN rows, 0 STOP, 0 stop violations.
- **G1:** Codex edited a run-folder helper at 17:42 while a disk pause was active. EU2 allows only the pause receipt to be written then. Codex disclosed it; no host file changed.
- **G2:** EU5 was not done by Codex; my identification above is input for chat, not its deliverable.
- **G3:** the static check lets `LoadClass`, `StaticLoadClass`, `TObjectIterator` and `FindFirstObject` through. None is in the probe today.
- **G4:** Codex's full EK1 scan was interrupted. My pre- and post-scans show 0 secret values.
- **O1:** `Stop()` still calls `RemoveFromRoot()` on raw pointers after the engine's exit purge, in both engines. It's only safe while those arrays are empty at exit.
- **Other independent results:**
  - P9 v2 18,264/18,264;
  - canary search by value from the launch second: 0 hits in 567 files, plus 0 in my own 397;
  - all 49 recorded child environments are clean (the two UBT ones never started);
  - 0 launches;
  - Codex's owner and kit suites re-run 159/159, 62/62 + 48 measured and 48/48;
  - the reflected-header check (EA11) passes on Paris and ReGate51, and the Baseline51 release control fails as expected;
  - the owner-slot temp files scanned in memory: clean.
- **Top 5 risks** (detail in the review):
  1. Disk dips and commit pressure could swallow the next build job the same way.
  2. Every host binary still has the exit crash until the rebuilds pass, plus the O1 path.
  3. Glass may be too common in the street to keep `T_Vetro02_N` out of frame.
  4. The static check's blind spots (G3).
  5. The arming prerequisites (clean launcher, the crash-upload proof, the missing Paris Editor makefile) are unmet.

**Owner actions**
- Restart the Epic Games Launcher cleanly (tray icon → Exit, then start it from the Start menu) and leave it running.
- Before the rebuild job, free memory and D: space: no Blender renders and no other heavy sessions. With no Blender running, Windows' `wlanext.exe` alone held about 9 GB and kept growing; a reboot would clear it, if you choose.
- If you know what drops D: by about 22 GiB at a time, pause it during the job.

**NEEDS-DECISION**
1. G1: accept the edit during the pause as a recorded deviation, and decide whether the next job must block edits while a pause is open.
2. How to schedule the six rebuilds on this machine (for example, one host per job, or an owner-cleared window).
3. The EU5 remedy, since `T_Vetro02_N` sits under `MM_Glass` and six glass instances.
4. Whether the rebuild job must also widen the static check (G3) and fix the `Stop()` path (O1).
5. Codex's 9 minutes of report-only work after the 9-hour limit.

These sign-ins are needed: several connectors (Google Calendar, GitHub, Slack and others) aren't authorized for this session; the PDF and meshy servers failed to connect. None was needed for this job.
api-equivalent cost (not billed on Max): $26.92 | turns: 217 | running total across all briefs: $4,979.02
