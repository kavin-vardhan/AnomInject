# 094-05 — Chat ruling: the job was cut off at 17:11 when the PC ran out of memory; 4.25 now compiles clean; a build memory gate, a clean recovery, and the owner's hand cleanup come next

**Date:** 2026-10-22, about 20:00 IST · **Author:** chat Claude (orchestrator)

**What happened (from the receipts; there is no 094-05 report):**
- **Brief 094-05 started at 15:35.** Code's own safety hook refused the cache delete, so nothing was deleted, `OWNER-DELETE-LIST.md` was written, and P9 v1 was kept (EE7.6).
- **Codex resumed at 15:52** and got through:
  - the entry proof against v1 (PASS);
  - the EE6 tooling fixes (30 self-tests);
  - EE3, with no PCH for the five plugin modules on 4.25;
  - the EE5 token proof;
  - the C++ fixes (`e421441`) and the ledger repair (`5bd368b`).
- **17:01, the 4.25 working build on `5bd368b`:** UHT PASS; **0 errors and 0 C++ warnings**; C2855 went from 9 to 0 and C5043 from 20 to 0. EE4 was not needed. 20 actions, 0 engine.
- **17:06, memory:** load at 98 %, 0.40 GiB free physical, 3.4 GiB free commit. The owner's three Blender processes held about 7.7 GB of working set and his UE 5.7 editor about 20 GB of commit. The page file on D: was at its 72 GiB system-managed maximum.
- **17:11, the 5.1 ReGate51 leg started:** UHT PASS, 29 actions, all outside the engine trees. UBT reported 8 first-include (IWYU) violations in guarded `.cpp` files, and then the compile ran (UBT 42980, cl 1572).
- **17:11:33,** Code's poll died with exit 4 from memory starvation. **Codex's events and Code's live feed both stop at 17:11:3x.** No report or exit code exists, and the PC probably crashed or froze.

## Verdict
- **094-05 is INTERRUPTED, not failed.** Its committed work stands, subject to the recovery checks in EF2.
- **This was our fault as much as the PC's.** The machine was already at 98 % memory, and Codex's own schedule (3 GiB commit and 1 GiB physical) let a 5.1 compile start anyway. **Our jobs must never be what takes the owner's PC down.**

## Rulings

### EF1. A build memory gate, for every build-mode run
- **Before each compile run,** free physical memory must be 6 GiB or more and free commit (`GlobalMemoryStatusEx` `ullAvailPageFile`) 12 GiB or more, on three consecutive polls about 20 s apart.
- **Parallelism:** `-MaxParallelActions=1` stays on both engines.
- **During a run:**
  - Sample every 5 s.
  - If free commit falls below 4 GiB, or free physical below 1 GiB, on two consecutive samples, stop **the port's own UBT process tree by PID**. That means only the processes your wrapper started: never `mspdbsrv` and never anything else.
  - Record the stop and go back to waiting.
  - A stopped run is not a D2 attempt. Clean its outputs (EF2) before the next run.
- **Wait limit:** 4 hours per wait. On timeout, end cleanly with NEEDS-DECISION "no memory window". That is not a failure and not a D2 attempt.
- **Self-tests:** synthetic readings must refuse at each threshold, and the in-run stop path must be tested with a synthetic sampler. Never test it by actually starving memory.
- **The owner is asked, not required,** to close Blender and UE 5.7 while our builds run. If he doesn't, the gate waits.

### EF2. Recovery from the interruption
- **Treat 094-05's last state as unverified until checked:**
  - the EA8 entry proof;
  - `git fsck --connectivity-only` on the common repository (read-only);
  - SQLite `PRAGMA integrity_check` on every ledger the job had open;
  - the 4.25 outputs against their 17:01 receipt hashes. If they don't match, re-run the 4.25 compile.
- **Clean the interrupted 5.1 outputs before the next 5.1 run.** That means everything the 17:11 action list could have written inside `D:\IA_Europa\ReGate51\StackOBot`, and nothing outside it. Do it by receipt, as port-owned scratch. ED2's "A4 decides on cleaning" is amended for this recovery only.
- **The 8 first-include violations** are ordinary include fixes. They must keep C3's rule: no UE5 expression is parsed in a disabled 4.25 translation unit.
- **094-05 is not a D2 attempt for anything,** and its 9-hour work limit starts again in the next job.

### EF3. The owner's cleanup and P9
**The owner deletes by hand,** following `code-receipts-094-05\cleanup\OWNER-DELETE-LIST.md`, before the watcher starts the next job.

**Code then checks:**
- the cache against the list, after two identical inventories 5 minutes apart;
- the keep list;
- the release baseline's 8 hashes;
- `D:\IntrusiveAnomalies\StackOBot\Content` against its 094-05 before-snapshot (the junction trap);
- `D:\IA_BankOverflow`, unchanged.

**Then it writes P9 v2** for whatever is gone, as in 094-05 Step 2. If nothing is gone, v1 stays.

If the owner hasn't deleted, he must not delete while the job runs.

## Next
- **094-06 is queued,** and starts no earlier than 20:35 IST, when Code's weekly allowance resets.
  - Code: machine and event-log check, the cleanup check and P9 v2, then a resume of thread `01a14334`.
  - Codex: recovery checks, EF1, the 5.1 include fixes, the 5.1 working compile to 0 errors, and then the final pair on one SHA.
  - Code reviews after Codex exits.
- **Owner, in this order:**
  1. Say whether the PC restarted or froze around 17:10.
  2. Do the hand delete.
  3. Start the watcher (or close and restart it) after 20:30.
  4. If you can, close Blender and UE 5.7 while our builds run.
