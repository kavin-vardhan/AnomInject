# ACK-VERDICT: 094-27 NEEDS-DECISION is ruled in _reviews\094-27-ruling.md (FH1-FH9): the kit and the real smoke session worked, and the STOP was a wrong row name in our own cleanup reader; FH2 fixes it and makes every reader pass real probe files; FH3 lets a non-crash leg failure continue the run; FH5 adopts the respawned crash monitor by its -MONITOR PID; FH6 allows -unattended on runtime legs; FH7 applies FD3's memory lines; 094-27b fixes, re-arms for 09427b and runs when the PC is idle.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-27b Europa Phase A runtime fix, re-arm and run when idle, A6 legs and A7 pictures: Codex fixes, rehearses, arms and runs the kit, Code reviews after

# 094-27b — Fix the runtime kit (FH2–FH8), re-arm it for 09427b, and run it for real when the PC is idle. Resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**This job launches the 4.25 editor and a standalone game, but only through the armed kit, and only Codex starts the kit.** You never launch anything, touch the editor or the game, or answer their dialogs.

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-27b\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex, check `m51` against P9's record** with read-only git (`GIT_OPTIONAL_LOCKS=0`):
  - HEAD `53bf725`;
  - index `526aba7f`;
  - 0 tracked changes;
  - the 15 untracked entries.

  **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Before launching Codex, also read 094-27's results:** `_reviews\094-27-codex-report.md`, `_reviews\094-27-code-review.md`, and the mailbox report. If either shows a protected-state change, a crash or an upload, stop with `VERDICT: NEEDS-DECISION`; you do not launch.
- **Launch hold:** launch once D: reads at least 40 GiB (EV3) and E: at least 15 GiB (EY4), on two polls.
- **Launch Codex as soon as your start checks pass.** The owner wants speed (FA5). Codex's real run waits for an idle PC by itself.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call, and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end, and starts with a `VERDICT:` line.
- **Launch Codex detached,** as 094-27 did. Poll with light, bounded condition loops, one at a time: about 10 minutes each, and up to 20 minutes while Codex is only waiting for an idle PC.
- **Never end the run while the Codex job is still running.** The cap is 24 h after launch. Past that, leave it running and report.
- **Keep context lean:** one compact line per poll. This job is long.

## Bootstrap
1. Read in full:
   - `_reviews\094-27b-request.md`;
   - `_reviews\094-27-ruling.md` (FH1–FH9);
   - `_reviews\094-27-request.md` (FB1–FB7);
   - `_reviews\094-26d-ruling.md` (FG1–FG4);
   - `_reviews\094-26c-ruling.md` (FF2 and FF5);
   - `_reviews\094-28-ruling.md` (FD2 and FD3);
   - `_reviews\094-26-ruling.md` (FC2–FC5);
   - `_reviews\094-25-ruling.md` (FA1 and FA5–FA8);
   - `_reviews\094-27-code-review.md` and the 094-27 mailbox report.
2. **Reuse your predecessor's tooling** in `code-receipts-094-27\`, and the older folders it drew on. That includes:
   - the Python 2.7 harness;
   - the kit and slot checks, and the live-gate compare;
   - the before and after lists;
   - the command-audit check and the `m51` check;
   - the disk and memory poller, and the poll script;
   - the cites file.

## The work

### Step 0. Start checks (FA6, lean)
- **Record:**
  - headroom (ED1, with EV3's D: lines and EY4's E: lines);
  - memory (physical and commit);
  - the toolchains (ED3);
  - the watcher.
- **Check `m51` against P9, and 094-27's results,** as above. Stop if either fails.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **Record:**
  - the feature HEAD;
  - the 4.25 manifest (27 files, `c501895c…`);
  - the launcher's state;
  - **your own before-lists:** a hash list of the Paris host's whole `Content` folder, and hashes of its `Config` files, plugin descriptors and `.uproject`;
  - the EditorRuns markers, including 31124;
  - the crash folders: the host's `Saved\Crashes`, the engine's, and `%LOCALAPPDATA%`'s (names only);
  - the drive-root listings of C:, D: and E: (top-level names only).
- **No EK1 pre-scan and no full P9 before-snapshot** (FA6).

### Step 1. The canary and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-27b\canary.txt`. The value goes nowhere else.
- **Preserve 094-27's report outputs in the run folder** before the resume overwrites them.
- **The resume prompt:** `resume-prompt-094-27b.txt` in the run folder, as before, UTF-8 with no BOM. Its contents, in order:
  1. `_reviews\094-27b-request.md`, verbatim;
  2. clearly marked Code-added context: `m51` status, 094-27's verdict and HEAD, memory, free space, the canary's name only, verified paths, the launcher's state, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm that a new turn started.
  - If the resume fails twice, start a fresh thread, as before.
  - A relay retry after a model-capacity error is not a failure. Record it.

### Step 2. Poll (W3)
- Use light condition loops until `exit-code.txt` appears.
- **Write one compact line per poll** in `code-receipts-094-27b\midjob-notes.txt`, by absolute path. Each line records:
  - free space, memory and commit;
  - the processes of ours (`UE4Editor`, a `-game` UE4Editor, `ShaderCompileWorker`, `CrashReportClient*`), with their PIDs and parents;
  - for any `CrashReportClient*` process, its `-MONITOR=` value from its command line.
- **Expected:** our editor and game processes appear only inside the kit's legs, one session per leg. The crash monitor names an owned PID (FH5). Record anything else, but never act on it.
- **A new or removed drive-root entry while Codex runs:** gather read-only writer evidence (names, sizes, counts, and the running program that names the path). Never open its files. Record it; don't act (EZ1).
- **Never steer Codex. Never touch the editor, the game, their windows or their dialogs.**

### Step 3. Collect and review (after Codex exits)
**Collect:**
- Codex's report to `_reviews\094-27b-codex-report.md`, with a header line;
- the ledger row.

**Write `_reviews\094-27b-code-review.md`:**
- **The rulings table:** FH1–FH9, FG1–FG4, FB1–FB7, FF1–FF5, FE1–FE5, FD1–FD6, FC1–FC9, FA1–FA8, EZ1–EZ6, EY1–EY8, EX1–EX6 (EX4 as amended by FH6) and EW1–EW4, with the earlier rulings in one row each where they apply.
- **FH2, yourself:**
  - for every row kind and field the readers require, find the line in the probe's C++ source that emits it, and list any requirement with no emitter;
  - run the fixed readers yourself on the real probe files (094-26c's EA_Core session, 094-26d's two sessions, 094-27's smoke), and on every real leg of this run.
- **The kit:**
  - read the diff;
  - re-run only the suites that cover changed or new code;
  - add your own can-fails for FH3 (a failing leg, then a passing one, and the second still runs), FH5 (a self-restarting `-MONITOR` stand-in is adopted and stopped first) and FH7 (FD3's lines).
  - Create no reparse point anywhere (FA1).
- **FH5 and FH6:**
  - check the `-MONITOR=` and `-unattended` citations in 4.25 source;
  - compare the kit's adopted-monitor table with your own polls.
- **The rehearsal, dry runs and live reading:** check them from their receipts, and do your own live compare once.
- **The real run, per leg, from the raw files yourself:**
  - the console lines against the kit's catalog;
  - the idle pre-check before each start, and the gate waits;
  - start, end, duration and exit;
  - the probe rows;
  - for A7, count the events per anomaly yourself from the probe rows (attempted, valid, applied, reverted, refused, invalid), and compare with the reader;
  - FH4: each leg's evidence hashes.
- **Each gate's verdict** (A3, A5, A6, A7 and A8, as FB1 scopes them): agree or disagree, with the reason.
- **The pictures:**
  - every screenshot named in the contact-sheet index exists, opens as a 1280×720 PNG, and belongs to the event its row names;
  - its AA echo is 0;
  - the contact sheets exist.
- **Nothing under Content, config, the descriptors or the `.uproject` changed,** from your own after-lists.
- **No uploads:**
  - the EU4 and FC4 switches are in every recorded argv, the `-game` legs included;
  - the session and crash-client logs show no data-router POST and no other upload;
  - list any new crash or ensure report folder, with the leg it belongs to.
- **FB7:**
  - run the Python 2.7 harness yourself on the changed in-editor and in-game scripts;
  - check the smoke session's receipt and its clean exit;
  - check that every UnrealBuildTool child is recorded with its argv and shows no build action (FD2);
  - check FF2's and FG2's classes.
- **The EditorRuns markers** per ET2, with 31124 unchanged.
- **The ledger:** C:, E: and 5.1-engine writes inside the EA4, EG1 and ET3 classes, and any foreign root entries recorded under EZ1.
- **The 4.25 manifest is unchanged** (27 files).
- **EU1:** check Codex's command audit, and spot-check it against the event log.
- **EL2:** check Codex's canary scan, and the canary by name in the recorded child environments, the adopted monitors included: 0 of each.
- **P9 v2:** check Codex's EA8 entry and exit proofs. Do no re-hash of your own (FA6).
- **Launches:** exactly the kit's legs, one session each, and nothing else.
- **Where you may write:** `D:\IA_Europa\Receipts\CodeReview09427b\` and `D:\IA_Europa\Temp\CodeReview09427b\`.
- **Your top 5 risks** for Phase B: the six basic anomalies, with 4.25-authored materials.

## Hard rules
- **No source edits, builds, launches, commits or pushes by you.** The only exceptions are suite and can-fail runs on owned scratch, and read-only scans.
- **Never run any kit for real.** Dry runs and rehearsals only, under your own review folders.
- **Never touch the editor, the game, their windows or their dialogs.**
- **Never open or read a file inside a foreign drive-root entry** (EZ1). Names, sizes and counts only.
- **You delete nothing.**
- **Never create a junction, symlink or other reparse point** anywhere under `D:\IA_Europa` or the run folder (FA1).
- **The only writes allowed to you:**
  - `code-receipts-094-27b\`;
  - the resume prompt, and the preserved copies of 094-27's report outputs;
  - the two `_reviews\094-27b-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09427b\`;
  - `D:\IA_Europa\Temp\CodeReview09427b\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging, power or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs, and never stop the editor, the game or the crash monitor: the kit's own limits do that.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
   - **GREEN:** all of these hold:
     - the kit was fixed, rehearsed and armed;
     - every leg ran;
     - gates A3, A5, A6, A7 and A8 (as FB1 scopes them) pass, by the readers and by your own counts;
     - the contact sheets exist;
     - nothing under Content, config, the descriptors or the `.uproject` changed;
     - no upload, and the canary is absent;
     - there is no blocking GAP;
     - P9 v2 is unchanged.
   - **NEEDS-DECISION:** any of these:
     - steps 2–4 failed before any real launch;
     - there was no idle window;
     - some legs didn't run;
     - a gate failed without a crash.
   - **RED:** a crash, a protected-state change, a Content change, or an upload.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, the pauses, the retries, any foreign root entries, and its VERDICT line.
5. **The changes and the run-up,** in 6–8 lines: FH2's real-file results, the can-fails, the rehearsal, the dry runs and the live reading.
6. **The real run, per leg:** gate wait, start, end, duration, exit and reader verdict.
7. **The gates:** a table with A3, A5, A6, A7 (per anomaly: valid events, applied, reverted) and A8, each with the reader's verdict and yours.
8. **The pictures:** the path of the contact-sheet index, and the 4 contact sheets.
9. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
10. **Owner actions:**
    - the eye check: which files to open, and what to look for;
    - anything left on screen.
11. **NEEDS-DECISION items,** or "none".
