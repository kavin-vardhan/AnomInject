# D:\IA_BankOverflow — what to delete by hand (brief 094-42d)

Written by Claude Code on 2026-11-03 at about 13:05 IST. **Nothing was deleted, moved or changed by this job.** Details for chat: `_reviews\094-42d-code-findings.md`.

**In short:** you get back **about 23.6 GiB on D:**. Nothing still needs these files. **But first see "Before you start" below.**

## Before you start
- **Only delete when no mailbox job is running.** Open `D:\IntrusiveAnomalies\_mailbox\watcher_status.txt` (or the mailbox dashboard). It must say `state=idle`.
  - It says `running` right now, because this check (094-42d) is the running job. Wait until its report has arrived.
  - This rule holds until chat has retired the old checks that still list this folder.
- **After deleting, tell chat.** Until chat confirms the fix, **don't double-click these two launchers:**
  - `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat`
  - `D:\IA_Europa\Launch\Europa425-FixtureAuthoring.bat`

  Their safety check still expects this folder. Without it they stop with an error before doing anything. Nothing breaks; they just won't run until the check is updated.

## The four items (everything inside `D:\IA_BankOverflow`)

| # | Path | Files | Explorer will show | Space you actually get back | Safe? |
|---|---|---|---|---|---|
| 1 | `D:\IA_BankOverflow\followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-OVERFLOW-UNQUALIFIED` | 88,147 | 63,251,421,988 B (58.907 GiB) | 16,955,228,160 B (**15.791 GiB**) | **Yes.** The paused J2 archive; you accepted rebuilding J2 from git if it ever resumes. Explorer shows more than you get back because 77,031 of its files are hard links to each other. |
| 2 | `D:\IA_BankOverflow\_r9315` | 1,356 | 4,899,658,417 B (4.563 GiB) | 4,903,383,040 B (**4.567 GiB**) | **Yes.** J2's last work folder (harness, builds, legs). Of its 97 scripts, 16 (75 KB in total) exist nowhere else; the rest are in the 094-05 courtesy zip or elsewhere. A J2 restart would rewrite those 16. |
| 3 | `D:\IA_BankOverflow\_binary_baselines` | 80 | 2,713,090,244 B (2.527 GiB) | 2,713,264,128 B (**2.527 GiB**) | **Yes.** 11 old archived builds from September–October bench rounds (m52, m53, 09102). Old journals cite them; nothing runs from them. |
| 4 | `D:\IA_BankOverflow\09102-final-binaries.zip` | 1 | 739,783,904 B (0.689 GiB) | 739,786,752 B (**0.689 GiB**) | **Yes.** A zip of the same three `09102-*` builds that item 3 already holds. |
| | **Total** | **89,584** | **71,603,954,553 B (66.686 GiB)** | **25,311,662,080 B (23.573 GiB)** | |

- **D: free space:** 37.56 GiB now, so **about 61.1 GiB** after. No file in this folder shares its data with any file outside it, so all of it is freed.
- **The E: twin is already gone.** That was the incomplete J2 archive `D:\IntrusiveAnomalies\_binary_baselines\followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-UNQUALIFIED`, with its `INCOMPLETE-ATTEMPT.txt` marker. You deleted it on 2026-10-16 at about 10:59 IST (ruling 094-01, EA9). Nothing to add.
- **No link points into this folder.** It is safe to delete it directly in Explorer: no junction or shortcut reaches anywhere else.

## How to delete (Explorer)
1. Check `watcher_status.txt` says `state=idle` (see above).
2. Open `D:\IA_BankOverflow` in Explorer.
3. Press **Ctrl+A**, then **Shift+Delete**, then confirm.
   - Use **Shift**+Delete. A plain Delete moves the files to the Recycle Bin on D:, and the space does not come back.
   - Explorer may take a minute to count the files. It will say about 66.7 GB; you get back about 23.6 GB.
4. Go up one level to `D:\` and Shift+Delete the now-empty `IA_BankOverflow` folder too.
5. Check D: shows about 61 GB free.
6. Tell chat "BankOverflow is deleted."

Nothing in the folder is read-only or hidden, and no path is too long for Explorer (the longest is 206 characters).

**Or tell chat to do it.** Chat can have Code delete it in a job. Explorer is simpler: Code's safety guard tends to refuse delete commands.

## What chat does after you delete
- Writes a new protected-file baseline (P9 v3) without this folder.
- Updates the kit's safety check to read it.
- Removes `D:\IA_BankOverflow` from the brief text and the job checks.

Then the two launchers above work again.
