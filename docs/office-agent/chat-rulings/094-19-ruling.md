# 094-19 — Chat ruling: a foreign disk dip pauses a job instead of stopping it; a read-only direct command is a recorded deviation; the launcher and upload designs are decided; re-run the fix and rebuilds

**Date:** 2026-10-27, about 14:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-19-codex-report.md` (12:54–13:29 IST, 2,108 s, exit 0). Its verdict: `RED | D: floor breach and an unfiltered read-only Git child; probe fix, rebuilds and research remain unstarted`.
- **Claude Code's review:** `_reviews\094-19-code-review.md`. 17 OK · 13 N/A · 1 GAP · 2 OPEN carried · 2 STOPs correctly raised · 0 stop violations. **Code's own read-only research** covers ET3, ET4 and ET6, cited line by line in 4.25 source.

## Verdict
**Both stops were correctly raised under the rules as written. Nothing was changed, built or launched.**
- **The D: floor:** D: fell to 44.72 GiB, against a 50 GiB floor, at 13:13. It fell again to 54.49 GiB at 13:53, after Codex had exited. Both dips came with the owner's Blender renders and recovered when a render ended. The port's own files were small, so the writer was not ours.
- **The direct `git status`:**
  - it was run from Codex's own shell, outside the filtered wrapper;
  - it inherited the canary and `CLAUDECODE`;
  - it was read-only, and the Paris index is unchanged;
  - the canary value appears in no file.

**Protected state:**
- P9 v2 18,264 of 18,264;
- `m51` equal to P9 ×5;
- the protected-state check PASS ×3;
- the engine trees unchanged;
- the manifests unchanged (27/39/44);
- 0 launches.

**Both rules were stricter than their intent, and that cost a round (mine again).** EU1 and EU2 narrow them.

## Rulings

### EU1. A read-only direct command is a recorded deviation, not a STOP (narrows EL2's STOP)
**EL2's STOP fires when the canary:**
- reaches a child that can write: a build, UBT, a compiler, the editor, the ledger, or a receipt writer;
- or appears in any file.

**A read-only inspection command run directly from Codex's shell:**
- covers `git status`, `log`, `diff`, `show`, `rev-parse` and `ls-files` with `GIT_OPTIONAL_LOCKS=0`, hashing, listing and reading;
- must write nothing;
- is **recorded as a deviation, and the job continues,** as long as the canary is in no file.

**The norm stays:**
- every external tool goes through the filtered wrapper;
- every job ends with a command audit, built from the event log, that classifies each direct command. A direct read-only command is a deviation; a direct write-capable command is a STOP.

**The 094-19 `git` call is closed as a recorded deviation.**

### EU2. A foreign-caused disk dip pauses the job (amends ED1 and EN5's floor STOP)
**When a drive falls below its ED1 floor,** and the port's own growth since the operation began would not, by itself, have crossed the floor:
- the job **pauses;**
- it stops its own running build or child tree by PID, as in EI2;
- it writes only the pause receipt;
- it waits.

**It resumes** when that drive is back above its warn line on three polls a minute apart.
- An interrupted build restarts from the beginning.
- That restart is not a D2 attempt.
- Its partial outputs are cleaned by receipt, as in EI2.

**After 4 hours without recovery,** the job ends NEEDS-DECISION "no disk window".

**A breach that the port's own writes would cause stays a hard STOP.**

**Self-tests, with can-fails:**
- a foreign drop pauses, then resumes;
- a port-caused drop stops;
- no recovery ends at the 4-hour limit;
- a drop during a build stops only our tree.

**D2 applies:** two attempts.

### EU3. The launcher (ET3, decided)
**There is no dedicated switch.**
- 4.25 `EditorEngine.cpp:607–632` opens the launcher silently at startup, unless it is already running (`:622`).
- `-DEMOMODE` changes about 20 editor behaviours. It is rejected.

**The design: the launcher must already be running before any editor leg,** so the editor spawns none.
- **Owner legs:** the card says to start the Epic Games Launcher first and leave it running.
- **Automated legs:** the kit checks that the launcher is running, and waits with a plain message if it isn't. The kit never starts the launcher itself.
- **The owner gate stops counting the launcher's own processes** (`EpicGamesLauncher`, `UnrealCEFSubProcess`, `EpicWebHelper`) as another Unreal program. It records them instead.
- **The launcher's writes are foreign (EB1).** ED1 still applies to them.

**This is built in the next arming job.**

### EU4. Uploads (ET4, decided)
**Every editor run gets** `-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False`.
- **Why:** ensures report unattended by default, and `ReportCrashForMonitor` returns at `WindowsPlatformCrashContext.cpp:624–633` when this value is false.
- **Nothing is saved:** command-line values are skipped on save (`ConfigCacheIni.cpp:1531/1543`), so no file changes.
- **Can-fail:** a kit argv without the switch refuses.

**Owner legs:** the card says "Close Without Sending" on any crash window.

**Automated legs: before the first one,** Codex proves from 4.25 source how a crash behaves with the switch in place. There must be no upload and no dialog that can hang the run. The proof includes exactly what `-unattended` would change. Chat rules on it then.

**This is built in the next arming job.**

### EU5. The texture (ET6)
**What the source shows:** `TextureDerivedDataTask.cpp:252` reports a multi-tile (UDIM) source, and the build then compresses only its first tile (`:266`). That texture can look wrong in pictures.

**In 094-20, read-only:**
- identify the texture from the asset files themselves, with a header scan of the Paris textures;
- A7's shot list then keeps it out of frame.

### EU6. Re-run 094-19's work as 094-20
**The order:**
1. EU2's tooling, first;
2. the probe fix (ES2 and ET5);
3. the static check and its mutants;
4. the six rebuilds;
5. EI4;
6. re-binding the manifests;
7. EU5's scan;
8. the EU1 command audit.

**Codex's unapplied draft in the run folder is a starting point, not evidence.**

**The kit stays unarmed.**

**Carried:**
- ER1's G3;
- ER3's bare UBT;
- the 8.3 and junction alias tests;
- EU3 and EU4, for the arming job;
- EO1's reader budget;
- A9;
- EJ1, EJ3 and EJ5;
- O3, for A8.

## Next
**094-20 is queued, with no launch.**

**Owner actions, whenever convenient:**
- close the old kit window;
- restart the Epic Games Launcher cleanly: tray icon → Exit, then start it normally from the Start menu, and leave it running. This releases our temp files, and the new launcher won't inherit our routing;
- keep extra Blender renders off while 094-20 builds, if you can. It isn't required any more, but the builds wait for memory.
