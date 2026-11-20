# ACK-VERDICT: 095-03's NEEDS-DECISION is resolved. Chat asked the owner on 2026-11-13 and he explicitly approved ("Yes, both") permanently deleting (1) the captured frames and masks (.png, .zlib, .u32) in E:\IA_BuildCache\0944*-evidence and (2) the old test evidence in E:\IA_Fixtures\Outline425\Receipts. Chat's answers to 095-03's decisions (a) to (d) are in this brief.
# SESSION: new
# MODEL: claude-opus-5-5
# EFFORT: high
# NAME: 095-03b Free E second pass (owner-approved frames and masks in old Europa evidence, Outline425 receipts), then rebuild the primary host and take the baseline capture readings - no Codex

# 095-03b — second E: pass, then the rebuild and baseline

You are Claude Code, running **headless** from the GDP mailbox in a **fresh side session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Why:** 095-03 freed 10.9 GiB and stopped correctly at its gate: E: has 31 GiB and the rebuild needs about 65.5. Most of the remaining evidence is frames and masks of 1 MB or less, which chat's own "keep small files" rule protected. **The owner has now explicitly approved deleting them** (2026-11-13; chat asked with the paths and sizes and that the delete is permanent; his answer: "Yes, both").

**Read first:**
- The 095-03 report and receipts: `D:\IntrusiveAnomalies\_mailbox\to_chat\095-03-e-drive-cleanup-rebuild-baseline.report.md` and `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-03\` (open `exit-receipt-095-03.txt` first; re-use its inventory method and scripts).
- The 095-03 brief (method, never-touch list): the file in `D:\IntrusiveAnomalies\_mailbox\processed\` ending `_095-03-e-drive-cleanup-rebuild-baseline.md`.
- The 095-01 brief, whose Parts 3.5, 4 and 5 you run in Part B: the processed file ending `_095-01-return-to-main-baseline.md`.
- `D:\IntrusiveAnomalies\_reviews\095-01-ruling.md`.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-03b\`.

## Chat's answers to 095-03's decisions
- **(a) E: space:** Part A below (both sources, owner-approved).
- **(b) D: floor:** for **this brief only**, D:'s stop floor is **30 GiB**. Reason: D:'s shortfall is the Windows page file, not project data, and this brief's build and capture outputs resolve to E:. Before building, list which outputs of the rebuild and the legs land on D: (resolve the junctions). If more than 2 GiB would land on D:, stop and report. Do not touch the page file or any machine setting. E:'s 50 GiB floor is unchanged.
- **(c) `09447u-package`:** keep it. 0.13 GiB is not worth a card pointing at nothing.
- **(d) Part B:** re-issued below.

## Part A — second pass on E: (owner-approved)

**Scope 1: `E:\IA_BuildCache\0944*-evidence`.** Delete files with the extensions `.png`, `.zlib` and `.u32`, **of any size**.

**Scope 2: `E:\IA_Fixtures\Outline425\Receipts`, and only that subfolder.** Delete `.png`, `.zlib` and `.u32` files of any size, and any other file over 1 MB (packages, archived EXE / DLL / PDB copies, zips, large logs, databases).

**KEEP in both scopes, never delete:**
1. The current packaged Europa Development build and every hardlink to it (as 095-03 verified).
2. Anything named by path in a `_reviews\*card*.md`, in `_reviews\094-47w-chat-ruling.md`, or in the GR6 and GR6b rulings as something to keep using.
3. Every folder a test script, selftest or the owner kit reads by path (095-03 kept two such fixture folders; keep them whole, pictures included). Scan for references the same way 095-03 did, now including the Outline425 tools.
4. Every other file of 1 MB or less that is not a `.png`, `.zlib` or `.u32`: receipts, scripts, logs, JSON, CSV.
5. Anything you are unsure about. Keep it and list it as "kept, unsure".

**Never touch:** the rest of `E:\IA_Fixtures\Outline425` (the fixture project, its `Plugins`, `Content`, `Config`, `Saved`, `Intermediate`, `Binaries`, and its registered plugin worktree), `E:\IA_BuildCache\StackOBot`, the `0944*-package` folders, `_binary_baselines`, `_bench_sessions_bank`, anything under `D:\IA_Europa`, and everything else on E: (the other large folders are the owner's personal data).

**How:** exactly as 095-03: inventory, `delete-manifest.csv` and `keep-manifest.csv`, check every delete entry against KEEP 1 to 5, check nothing is running from or holding the paths (never kill; skip what is in use), delete exactly the manifest, remove the directories left empty, verify the kept set, re-verify the current Europa package (34 hashes; starts with `-game -nullrhi -ExecCmds="quit"` and exits 0 with its user folder and log pointed at your receipts). Record free GiB on C:, D: and E: before and after.

## The gate between A and B
Continue to Part B **only if** E:'s free space, minus the 14.54 rebuild figure and about 1 for the two legs, stays at or above 50 GiB, **and** D: is at or above 30 GiB with the D:-output check above passing. Otherwise stop after Part A and report NEEDS-DECISION with the numbers.

## Part B — the rebuild and the baseline
Run these parts of the 095-01 brief **exactly as written there:**
- **Part 3 step 5:** compare the primary host and the bench-fixture plugin with what the release proof used; report differences; align only by a plain checkout of an existing commit on a clean tree.
- **Part 4:** rebuild Editor and Game. 0 errors.
- **Part 5:** write `predictions-095-03b.txt` before launching; run legs B1 and B2; report the readings table. The input-idle wait stays waived today (`idle_wait_waived=true`); the other admission checks stay.

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
  2. **Part A:** size table before and after per scope, manifest counts, the "kept, unsure" list, free space per drive.
  3. **Build:** result, warning count, hashes. **Host and fixture differences.** What landed on D:.
  4. **Baseline:** predictions against readings, per leg.
  5. **Safety,** or "none". **Decisions chat must make,** or "none".
