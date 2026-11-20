# 094-24 — Chat ruling: the G1 fix works on the real branch, but a false STOP (the owner's new OBS folder on E:) ended the job before the re-arm; ED5 gets attribution; 094-25 finishes the fix and re-arms tonight; the real run (094-26) moves to tomorrow night

**Date:** 2026-10-28, about 23:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-24-codex-report.md` (17:49–21:37 IST, 13,629 s over 2 tries, exit 0). Try 1 ended at about 20:52 on a model-capacity error, and the relay resumed it at 21:02. Its verdict: `NEEDS-DECISION | ED5 STOP at E:\OBSVideos; 09424 not re-armed; zero Unreal launches.`
- **Claude Code's review:** `_reviews\094-24-code-review.md`. 34 rows: 28 OK · 2 N/A · 1 OPEN (carried) · 1 NOT DONE (the re-arm) · 1 STOP (ED5, foreign) · 1 GAP (G2). Also O1–O5.
- **Mailbox report 094-24:** NEEDS-DECISION, 351 min.
- **Code's read-only evidence on the new folder:** `code-receipts-094-24\review\obsvideos-writer.txt`.
- **The owner, 10-28 evening:** he cleared E: (over 300 GiB free) and confirmed that `E:\EpicGames\UE_4.25`, `E:\Unreal Projects\StylizedParisStreet`, `E:\CodexData` and `E:\IA_BuildCache` were not touched.

## Verdict
**What was done is ACCEPTED as far as it went. It is uncommitted, so it is not yet bound.**
- **G1 is fixed:** `EUROPA_AUTHORING_MARKER` is gone, and the marker path is derived from the recipe state.
  - Through the real `.bat`, the disk supervisor recognised each recipe's marker, and no environment carries the old name.
  - The environment-read check passes, with 16 named reads, and catches 5 of Code's mutants.
- **The real-branch rehearsal works:** a harmless stand-in replaced the editor, and every root was under owned scratch. Between them, Codex's 4 cases and Code's own 5 cover:
  - both recipes passing, each reaching `clean exit and output checks passed` and becoming spent;
  - a partial output in either recipe, quarantined by receipt, with EA_Travel not started after a failed EA_Core;
  - an extra undeclared file, refused;
  - a missing `probe_stop`, refused;
  - the deadline, which stops the whole tree, a detached child included, while a sibling survives;
  - a child named like the crash reporter, stopped with the tree, while a same-named sibling outside it survives.
- **EY2:** the exemption by install folder matched Code's own reading on 3 live polls: 9 programs (the launcher, 3 web helpers and 5 overlays), all inside the two Epic folders, with 0 blockers. Code's can-fails pass 29/29.
- **EY3:**
  - the 60 s unreadable-reading stop passes 12/12;
  - UnrealPak and UnrealLightmass block;
  - the start gate shows `Waiting: 12 GiB free commit.` below its line.
- **EY4:** the start check passes at exactly 15 GiB on E:, and stops 1 byte below.
- **0 Unreal launches. The protected state is unchanged:**
  - P9 v2 18,264 of 18,264, and `m51` equal to P9 at 4 checks;
  - 110 of 110 bound files, and Paris Content unchanged;
  - the canary found 0 times in 2,652 files;
  - EK1 found 0 secret values in 48,023 files;
  - the command audit: 118 commands, 1 direct read-only, 0 direct write-capable.

**The STOP was a false alarm, and the rule was mine.**
- **What happened:** at 19:42:43 IST a new top-level folder, `E:\OBSVideos`, appeared. ED5 makes any new drive-root entry a STOP, so Codex stopped implementing and closed out. Under the rule as written, that was correct: it had no evidence of the writer.
- **Code's read-only evidence:** the owner's OBS Studio, started from Explorer at 19:35, has `E:/OBSVideos` as its recording path, and the folder holds its recordings. The folder is the owner's.
- **Why the rule was too strict:** ED5 exists to catch the port writing outside its roots. Like EU2's floors, it should judge who wrote, not only what changed.

**State left by the stop:**
- 15 tooling files uncommitted (11 modified, 4 new), hashed in `D:\IA_Europa\Receipts\09424\stop-closeout.json`, with the diff in `…\09424\stop-state\tracked-tooling.diff`;
- `feat/europa-port` still at `97de9d4`;
- no 09424 arming;
- the 09423 arming refused by the candidate. 11 of its bound tool rows no longer match, and its 15 real paths are absent.

## Rulings

### EZ1. ED5's root rule is attributed (Code's D1, option b; amends ED5, as EU2 did for floors)
**A new top-level entry on C:, D: or E:, outside the port's declared roots, is foreign when both hold:**
1. no process of the port's own tree has the path in its command line, working folder or environment;
2. the entry holds no port-pattern file. Port patterns are our receipt, ledger, log, probe and script names, DDC and shader-cache files, and Unreal packages. Codex lists them; Code checks the list.

**A foreign entry is recorded and the job continues:**
- path, creation time, file counts and sizes by extension;
- any writer identified read-only, as Code did for OBS;
- its files are never opened or read.

**Otherwise the STOP stands.** On D:, a new entry inside or beside `D:\IA_Europa` or the run folder stays port-attributed.

**A removed top-level entry** is foreign and recorded when it is outside the port's roots and outside every P9-protected path. The port deletes only by receipt, inside its own roots, so it cannot have removed anything else. **A removed or changed P9-protected path is still RED.**

**`E:\OBSVideos` is closed as foreign,** on Code's evidence. So are the owner's E: removals on 10-28, provided P9 v2 passes.

**Can-fails:**
- a new root folder holding only foreign-pattern files is recorded, and the job continues;
- one holding a port-pattern file is a STOP;
- one named in a port process's command line is a STOP;
- a removed foreign root folder is recorded, and the job continues;
- a removed P9-protected root is RED.

### EZ2. A start-line miss at admission waits instead of stopping (amends the kit's admission check)
**At the kit's admission, a drive between its floor and its start line now waits** for the start line, under the measured gate's 4-hour limit, with a `Waiting:` line naming the drive. It no longer ends the run.
- **Why:** foreign D: dips of about 20 GiB (page-file growth while the owner works) refused one rehearsal today, at 37.31 GiB against D:'s 40 GiB start line. In the real run, that refusal would end the night.
- **Below a floor:** EU2's pause, unchanged.

**The disk lines stay as EY4 set them:** C: 13 / 11 / 10, D: 40 / 30 / 25, E: 15 / 12 / 10 GiB. The owner cleared E: on 10-28 (over 300 GiB free), so E: needs no further change.

### EZ3. 094-24's retained state is 094-25's starting point (Code's D2)
- **The 15 uncommitted tooling files,** exactly as hashed in `D:\IA_Europa\Receipts\09424\stop-closeout.json`, are the accepted starting state. They are not rebuilt. Any other difference is a STOP.
- **EA8's entry proof** accepts exactly that diff in the port worktree.
- **First,** Codex verifies the diff by hash and re-runs the suites that passed on it in 094-24 (EY, fixture, measured gate, session and static). **Then** it commits the diff as-is, as one commit (W4).
- **G2, EZ1, EZ2 and O1** follow in separate commits. **Every suite, G2's included, must pass on the final commit** before the re-arm.
- **The completed rehearsal cases** stay as evidence. All rehearsal cases run again on the final commit, so the bound code is the rehearsed code.

### EZ4. 094-25, tonight: finish, rehearse, re-arm for 09425, dry-run. No launch
**In order:**
1. EZ1, EZ2, G2 and O1 in the tooling, with their can-fails;
2. every rehearsal case again on the final commit, Codex's four and Code's five, so the bound code is the rehearsed code;
3. re-arm for 09425, with fresh slots, EW2's scope, and the 09423 arming refused;
4. dry runs of both recipes;
5. the live, observe-only gate reading (EY2).

D2 applies: two attempts per harness problem.

### EZ5. 094-26: the real run, on the 09425 arming
- It runs under 094-23 EY7 as written: the gate, the attempts, the evidence, the stops and the verdict meanings.
- **The window:** tomorrow night, 10-29: not before 23:00 IST, and no new start after 04:30 IST.
- **If the owner says his PC is free during the day,** chat may move the window by ruling.

### EZ6. Code's GAP and observations
- **G2: every retained suite passes in full before the re-arm.**
  - The 09417 EQ suite, and Code's own older can-fail sets, fail only because their synthetic readings lack the new commit reading and image paths.
  - Update those readings to carry both, without changing any case's intent.
  - If a case can't be kept without changing its intent, end NEEDS-DECISION with that case.
- **O1:** extend the environment-read check to the four copy-based read forms, with a mutant for each.
- **O2:** the rehearsal console prints its actual deadline. Fix it if it is a one-line change; otherwise record it.
- **O3:** a timing test that fails only under disk load is re-run once on a quiet poll. A second failure is a GAP.
- **O4 and O5:** accepted. An unusual launcher install path makes the gate wait, which is fail-safe.
- **Not adopted:** a rule that the owner must not create or delete top-level folders while a job runs. EZ1 makes it unnecessary, and the owner's PC is his.

## For the owner
**Tonight:** nothing. The fix job runs without opening anything, so use your PC normally.

**Tomorrow night, before you leave the PC:**
1. **Close Blender and OBS completely.**
2. **Close anything Unreal or <other-home-project>,** including anything opened from the Epic launcher. Pause the <other-home-project> watcher.
3. **Keep the Epic Games Launcher open.**
4. **D: needs 40 GiB free or more,** and E: 15 GiB.
5. **Leave the PC and the Claude app on.** No restart.
