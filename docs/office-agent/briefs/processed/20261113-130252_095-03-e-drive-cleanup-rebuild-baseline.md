# ACK-VERDICT: pre-staged by chat on the owner's explicit go of 2026-11-13 for the E: cleanup. This brief is independent of 095-02's plans: whatever 095-02 reported (GREEN, NEEDS-DECISION or RED), this brief runs, and chat rules on 095-02 separately.
# SESSION: new
# MODEL: claude-opus-5-5
# EFFORT: high
# NAME: 095-03 Free E by manifest (old Europa home evidence and superseded packages), then rebuild the primary host and take the baseline capture readings - no Codex

# 095-03 — free E: by manifest, then the rebuild and baseline that 095-01 could not run

You are Claude Code, running **headless** from the GDP mailbox in a **fresh side session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Why:** 095-01 stopped correctly because E: had 22.69 GiB free against its 50 GiB floor, and the primary host needs a full rebuild (about 14.5 GB). **The owner has explicitly approved this cleanup** (2026-11-13, asked twice by chat; his answer: "Yes, delete them", for the old Europa home-test evidence and superseded packages in `E:\IA_BuildCache\0944*-evidence` and `0944*-package`, knowing the delete is permanent). The Europa port is delivered (`feat/europa-port` = `2828f6a`) and the rulings already recorded those runs' numbers.

**Read first:**
- `D:\IntrusiveAnomalies\_reviews\095-01-ruling.md` and `D:\IntrusiveAnomalies\_reviews\095-00-ruling.md` (§1, §5).
- The 095-01 report and pre-flight: `D:\IntrusiveAnomalies\_mailbox\to_chat\095-01-return-to-main-baseline.report.md`.
- The 095-01 brief, which Part B below re-uses: the file in `D:\IntrusiveAnomalies\_mailbox\processed\` whose name ends `_095-01-return-to-main-baseline.md`.
- For the method, last week's D: cleanup: the processed brief ending `_094-49-europa-cleanup.md` and its report in `to_chat\`.

**State now:** the primary checkout is on `master` = `ebc34ed`, clean apart from 15 untracked files. Its `Binaries` do not match its source until this brief's rebuild.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-03\`.

## Part A — free E: by manifest (owner-approved)

**Scope: only** the folders `E:\IA_BuildCache\0944*-evidence` and `E:\IA_BuildCache\0944*-package` (the set your 095-01 report listed, 52.77 GiB). Nothing else on E:, and nothing on C: or D:.

**KEEP, never delete:**
1. **The current packaged Europa Development build:** the package 094-48c and 094-48d ran and 094-49 re-verified (34 files, by SHA-256), plus every file hardlinked to it. If it lives in one of the in-scope folders, that folder's package files stay.
2. **Anything named by path** in a `_reviews\*card*.md`, in `_reviews\094-47w-chat-ruling.md`, or in the GR6 and GR6b rulings as something to keep using.
3. **Every file of 1 MB or less** (receipts, scripts, logs, JSON). They are the audit trail.
4. **Anything a tool script or the owner kit reads by path** (094-49 found some; check the same way).
5. **Anything you are unsure about.** Keep it and list it as "kept, unsure".

**DELETE, if no KEEP rule protects it:** files over 1 MB in the in-scope folders: captured frames and masks, `.u32` / `.zlib` evidence, superseded packages, archived EXE / DLL / PDB copies, zips, large logs.

**How (the same as 094-49):**
1. Inventory the in-scope folders: sizes, link counts, file IDs. Write `inventory.csv` and a size table per folder.
2. Build `delete-manifest.csv` (path, size, last write, reason, rule) and `keep-manifest.csv`. Check every delete entry against KEEP 1 to 5.
3. Check that no process has an image or an open handle under a manifest path. Never kill anything. If a path is in use, skip it.
4. Delete exactly the manifest entries, then the directories left empty inside the deleted trees.
5. Verify the KEEP set afterwards: the kept package still matches its 34 hashes and starts with `-game -nullrhi -ExecCmds="quit"` and exits 0, with its user folder and log pointed at your receipts.
6. Record free GiB on C:, D: and E: before and after.

**Never touch:** `E:\IA_BuildCache\StackOBot` (the primary host's junction targets), the J2 worktrees and hosts (`_r53_src`, `_r53_host`, `_r52p_src`, `_r933`, and the `_r917` registration), `_binary_baselines`, `_bench_sessions_bank`, anything under `D:\IA_Europa`.

**Also report, read-only:** the ten largest other folders on E: by size, with a one-line note on what each is. Chat may need more room later. Delete none of them.

## The gate between A and B
Continue to Part B **only if** E:'s free space, minus your estimate for the full rebuild and both capture legs, stays at or above 50 GiB. Use the measured 14.54 GB rebuild figure unless you have a better one, and say which you used. If it does not hold, stop after Part A and report NEEDS-DECISION with the numbers.

## Part B — the rebuild and the baseline (from 095-01)
Run these parts of the 095-01 brief **exactly as written there:**
- **Part 3 step 5:** compare the primary host and the bench-fixture plugin with what the release proof used; report differences; align only by a plain checkout of an existing commit on a clean tree.
- **Part 4:** rebuild Editor and Game. 0 errors.
- **Part 5:** write `predictions-095-03.txt` before launching; run legs B1 and B2; report the readings table. The input-idle wait stays waived today (`idle_wait_waived=true`); the other admission checks stay.

Then record it on `master`:
- add the baseline readings to `docs/sessions/2026-11-13-095-01-return-to-main.md` and update the `CLAUDE.md` status entry: the host is rebuilt, its binaries match `master`, and bench legs may run again;
- one docs commit, the pre-push scan on that commit, push `master` (fast-forward), `ls-remote`.

## Rules
- **No product source edits.** No Codex. No `--force`, `clean`, `reset --hard` or `stash`.
- **Delete only by the Part A manifest.** Nothing else, on any drive.
- **Never touch:** the `m51` ref, `fix/followup-capture`, `feat/europa-port`, tags, the dashboard repo, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`, `ToCodex\`.
- **Never kill a process by name.** No machine settings. Never print a token. Codenames only.
- On any failed check or real fork: **report and stop.** Never iterate on a failure.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. End only with the final message below. Target: under 2.5 hours.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line: no number in front, no code fence around the message. GREEN means Part A is done and verified, the gate held, the rebuild passed, B1 ran with its readings reported, and the docs commit is pushed.
- Then:
  1. **For the owner (3 lines, plain words):** how much space was freed and what was deleted; did the test game rebuild; what the baseline capture showed about skipped frames.
  2. **Part A:** size table before and after, manifest counts, the "kept, unsure" list, the largest other folders on E:.
  3. **Build:** result, warning count, hashes. **Host and fixture differences.**
  4. **Baseline:** predictions against readings, per leg.
  5. **Safety,** or "none". **Decisions chat must make,** or "none".
