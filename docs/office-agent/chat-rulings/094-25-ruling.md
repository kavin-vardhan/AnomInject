# 094-25 — Chat ruling: the kit is finished, rehearsed and armed for 09425; Code's own test junction must be removed first (FA1); the real run (094-26) starts now and runs whenever the PC is idle (FA5); lean checks from now on (FA6)

**Date:** 2026-10-29, about 07:25 IST; FA5–FA7 added about 09:10 IST; FA8 about 13:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-25-codex-report.md` (10-28 23:50 – 10-29 05:33 IST, 20,586 s, 1 try, exit 0). Its verdict: `GREEN | 09425 kit re-armed, fully rehearsed and dry-run on bound tooling; closeout passes; zero Unreal launches`.
- **Claude Code's review:** `_reviews\094-25-code-review.md`. 42 rows: 36 OK · 2 N/A · 1 OPEN (carried) · 1 NOT DONE (its own rehearsal) · 2 rows tied to D1 · 0 protected-state changes. Also O1–O7.
- **Mailbox report 094-25:** NEEDS-DECISION, 476 min.

## Verdict
**Codex's work is ACCEPTED. The 09425 kit is the one tonight's run uses.**
- **EZ3:** the retained 15 files were committed unchanged (`a44c19d`), byte for byte against 094-24's receipt. Everything else came in its own commits.
- **EZ1:** new and removed drive-root entries are attributed, with metadata only and no file ever opened. Code's can-fails pass 31/32 plus 4/4; the one miss was its own harness bug.
- **EZ2:** a drive between its floor and its start line waits at admission, with its own `Waiting:` line, and passes when it recovers. Code: 27/27.
- **G2:** every retained suite passes on the final commit. Code re-ran all 22 entry points, including the 159-case EQ suite.
- **O1 and O2:** the four copied environment-read forms are refused, and the rehearsal console prints its real deadline.
- **The rehearsal, on the armed code `a8d3425`:** all 7 cases passed on their first attempt, through the real `.bat` and the real branch, with 9 harmless stand-in sessions on owned scratch. Code checked every case from its receipts:
  - both recipes pass and become spent;
  - a partial output in either recipe is quarantined, and EA_Travel never follows a failed EA_Core;
  - an extra undeclared file is refused;
  - a missing `probe_stop` is refused;
  - the deadline stops the owned tree and a detached child, and a sibling survives;
  - a crash-reporter-named child is stopped, and a same-named sibling survives.
- **The arming:** 09425 equals EW2's scope exactly, with fresh and unused slots. The 09423 arming and a never-armed 09424 job both refuse.
- **Both armed dry runs passed,** with 0 launches.
- **The live gate reading** excluded the launcher and its 8 helpers by install folder, with 0 blockers. Code's own reading matched on 6 polls.
- **Protected state:**
  - P9 v2 18,264 of 18,264, and `m51` equal to P9;
  - 110 of 110 bound files, and Paris Content unchanged;
  - the canary found 0 times in 6,675 changed files, and 0 times by name in 430 child environments;
  - EK1 found 0 secret values in 54,879 files;
  - the command audit: 207 wrapped, 1 direct read-only, 0 direct write-capable;
  - 0 launches.

**One blocker, created by the review itself (Code's D1):**
- Code's EZ1 can-fail created a directory junction in its own review scratch:
  - **the junction:** `D:\IA_Europa\Temp\CodeReview09425\ez1-canfails\roots\OwnerWithJunction\link`;
  - **its target:** `…\roots\junction-target`, which holds one 9-byte synthetic file;
  - **created:** 05:36, after Codex exited.
- The kit's ledger refuses any reparse directory under `D:\IA_Europa`. That is the intended fail-safe. So Code's own dry run and its 5 rehearsal variants stopped at the first prelaunch check, with 0 launches. Tonight's run would stop there too.
- **It is not a kit defect.** Codex's arming, rehearsal and dry runs all finished before the junction existed.

## Rulings

### FA1. The junction is removed first, by Code, in 094-26 (Code's D1)
**The removal:**
- It is the only delete authorized: exactly `D:\IA_Europa\Temp\CodeReview09425\ez1-canfails\roots\OwnerWithJunction\link`, the link only.
- **First, confirm** that it is a directory junction whose target is `…\roots\junction-target`.
- **Then remove it** with `cmd /d /c rmdir "<link>"`: no `/s` and no `/q`.
- **Then confirm** that the link is gone, and that the target folder and its file are unchanged (size and SHA-256, before and after).
- The receipt goes in `code-receipts-094-26\fa1-junction-removal.json`.
- **If anything differs** (not a junction, a different target, or more than one reparse point in Code's review folders), stop NEEDS-DECISION and launch nothing.

**Then Code's own dry run of the real entry point must pass,** on throwaway slots:
- both recipes report `armed dry run passed`;
- 0 launches;
- the real slots stay absent.

A failure is NEEDS-DECISION: don't launch Codex.

**Skipped:** Code's 5 rehearsal variants. Codex's 7 cases ran on the exact armed code, and Code verified them from their receipts.

**From now on:** no tool of ours, Codex's or Code's, creates a junction, symlink or other reparse point anywhere under `D:\IA_Europa` or the run folder, test scratch included. Reparse can-fails use a synthetic model only.

### FA2. Observations
- **O1:** `{**os.environ}[N]` still escapes the environment-read check. No helper uses it. **Carried:** add it with the next tooling change.
- **O2:** EZ1's pattern list includes generic names (`report.md`, `summary.json` and so on) and any Unreal package. A new foreign root holding one is a STOP.
  - **Fail-safe, kept for tonight.**
  - **Carried:** narrow the generic names after the real run.
  - The owner is asked, for tonight only, not to create new top-level folders.
- **O3:** a new top-level `D:\` entry stays port-attributed, as ruled. A removed P9-protected root returns a STOP with a RED record; the verdict reads from the record. Accepted.
- **O4 (memory):** the owner's two `run_kgen.py` jobs at 01:00 drove free commit to 0.43 GiB and physical memory to 0.23 GiB.
  - The start gate (12 GiB commit, 6 GiB physical) would simply wait.
  - A session already running would be stopped at 6 GiB commit.
  - **The owner's checklist names those jobs.**
- **O5, O6 and O7:** accepted. O6 is a UE 5.5 crash-report file of the owner's, classified foreign.

### FA3. 094-26: the real authoring run
**What runs:** 094-23 EY7, on the 09425 arming, after FA1.

**The window and the limits:** FA5's (they replace the original 23:00–04:30 window).

**Verdict meanings:** EY7's.

### FA4. Carried
- **For A7:**
  - EO1's reader budget;
  - EV6's glass baseline;
  - EJ5's 6 GiB physical rule;
  - EX2's note that measured legs get their own in-session rules;
  - O1, the runtime clean exit, which 094-26 proves.
- **O2b.**
- **FA2's O1 and O2.**

### FA5. The owner wants speed: 094-26 starts now and runs whenever the PC is idle (amends FA3 and EY7's window)
**The owner, 10-29 about 08:45 IST:** "This is going dead slow, please speed up."

- **094-26 is staged now,** at about 09:15 IST. Its window runs from Codex's start until 06:00 IST on 10-30. The 23:00 start line is withdrawn.
- **Before each start of the entry point,** Codex waits until the kit's observe-only collector shows:
  - the PC idle for 10 minutes;
  - the memory, GPU and process conditions met.

  Only then does it start the entry point, so the 3 prelaunch slots are spent only on real idle windows.
- **The kit's own measured gate is unchanged,** and it still decides when each session starts.
- **The owner's quiet slot, 10-29:** nothing heavy, no entry-point start and no session from 10:00 to 10:30 IST (owner, about 09:25 IST).
- **Time limits for this job:**
  - Codex's limit is the window itself, because most of it is waiting;
  - Code's cap is 24 h after launch.

### FA6. Lean checks from 094-26 on (amends the per-job cadence of EA8, EK1 and Code's review)
**Why:** each job has spent most of its 6–8 hours re-proving things this work cannot touch. The port is now its own 4.25 branch (EY5), so the 5.1 release, `m51`, StackOBot and the 5.1 engine are outside everything a 4.25 job does.

**Per job, from now on:**
- **Protected state:** the quick checks only. These are `m51` against P9 by git, the release baseline's 8 hashes, and the protected-state check.
  - Codex runs its full EA8 proof once, at entry.
  - The full exit proof runs only at a milestone (the end of A6, A7 or Phase B) or after a STOP.
  - Code's independent full re-hash is dropped. Codex's proof is enough.
- **Secrets:**
  - every job: the canary scan of new and changed files from the launch second;
  - only at milestones and before any push: the full EK1 scan.
- **Code's review covers:**
  - the job's own change and its evidence;
  - the suites that touch changed code;
  - its own before and after lists where outputs matter.

  It no longer re-runs every older suite and can-fail set, or repeats Codex's full scans.
- **Foreign activity never ends a job.** Disk dips, new or removed folders that aren't ours, and the owner's programs are recorded, waited out, or both.
- **Only four things end a job early:**
  - a protected path changed;
  - our process wrote outside its allowed places;
  - a crash of ours;
  - a secret value in a file.
- **Unchanged:** the in-session acting limits (commit, floors and the deadline) keep protecting the machine.

**094-26 is the end of A6, a milestone.** So it keeps Codex's EA8 exit proof, and drops the full EK1 scan and Code's re-hash.

### FA7. Jobs chain
- When the next step is clear, chat pre-stages the next brief while the current job runs.
- The watcher starts it as soon as the current job ends GREEN, and holds it on NEEDS-DECISION or RED (its existing verdict gate).
- No job waits for chat when nothing needs a decision.

### FA8. 094-26 was interrupted by a PC restart; 094-26r re-runs it
- **What happened:** at about 13:10 IST on 10-29 the PC restarted, while Codex was still waiting for an idle PC.
  - The watcher recovered 094-26 as RED (interrupted).
  - No editor started, and no slot was used.
  - 094-27 was held, as designed.
- **094-26r re-runs 094-26 as written,** under FA3, FA5 and FA6, with:
  - FA1 already done: Code verifies it and doesn't repeat it;
  - no repeat of Code's dry run, which passed at 10:46;
  - a fresh Codex entry proof.
- **094-27 is re-queued behind it,** unchanged.

## For the owner
**094-26 starts now.** It runs whenever your PC has been idle for 10 minutes: when you leave for work, or tonight.

**Before you leave the PC (now, if you're heading out):**
1. **Close Blender and OBS completely.**
2. **Stop your kimodo jobs** (`run_kgen.py`) and any Codex job of yours.
3. **Close anything Unreal or DCDemo,** including anything opened from the Epic launcher. Pause the DCDemo watcher.
4. **Keep the Epic Games Launcher open.**
5. **If you can, don't make new top-level folders** on C:, D: or E: today.
6. **Leave the PC and the Claude app on.** No restart.

**If you come back while it's running:**
- If an Unreal editor window is open, don't touch it. It closes itself within 30 minutes.
- If a crash box shows, click **Close Without Sending**.
