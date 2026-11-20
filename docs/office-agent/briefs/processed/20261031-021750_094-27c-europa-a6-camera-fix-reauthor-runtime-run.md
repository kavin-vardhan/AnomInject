# ACK-VERDICT: 094-27b RED (a crash) is ruled in _reviews\094-27b-ruling.md (FI1-FI6): the FH fixes and the monitor adoption worked in two real sessions; the Load leg's read-back found the A6 cameras rolled -12 degrees because the recipe used a positional unreal.Rotator (roll, pitch, yaw; gotcha G410), and the driver's failure path then looped load and quit until the editor crashed on exit; FI2 makes rotators keyword-only and re-authors both maps, FI3 makes the failure path single-shot, FI4 lets a first runtime crash continue the run; 094-27c fixes, re-authors, re-arms and runs when the PC is idle.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-27c Europa fix the A6 camera and the driver failure path, re-author both maps, re-arm and run the runtime legs when idle: Codex runs, Code reviews after

# 094-27c — Fix the A6 camera (G410) and the driver's failure path, re-author EA_Core and EA_Travel, re-arm the runtime kit, and run it for real when the PC is idle. Resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**This job launches the 4.25 editor and a standalone game, but only through the two armed kits (A6 authoring, then runtime), and only Codex starts them.** You never launch anything, touch the editor or the game, or answer their dialogs.

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-27c\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex, check `m51` against P9's record** with read-only git (`GIT_OPTIONAL_LOCKS=0`):
  - HEAD `53bf725`;
  - index `526aba7f`;
  - 0 tracked changes;
  - the 15 untracked entries.

  **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Before launching Codex, also read 094-27b's results:** `_reviews\094-27b-codex-report.md`, `_reviews\094-27b-code-review.md` and the mailbox report. If either shows a protected-state change or an upload, stop with `VERDICT: NEEDS-DECISION`; you do not launch.
- **The ruling was written before your predecessor's review was finished.** If that review raises a blocking GAP that the ruling doesn't cover, and it is not a protected change or an upload, record it in the resume prompt's Code-added context and launch anyway.
- **Launch hold:** launch once D: reads at least 40 GiB (EV3) and E: at least 15 GiB (EY4), on two polls.
- **Launch Codex as soon as your start checks pass.** The owner wants speed (FA5). Codex's real sessions wait for an idle PC by themselves.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call, and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end, and starts with a `VERDICT:` line.
- **Launch Codex detached,** as 094-27b did. Poll with light, bounded condition loops, one at a time: about 10 minutes each, and up to 20 minutes while Codex is only waiting for an idle PC.
- **Never end the run while the Codex job is still running.** The cap is 24 h after launch. Past that, leave it running and report.
- **Keep context lean:** one compact line per poll. This job is long.

## Bootstrap
1. Read in full:
   - `_reviews\094-27c-request.md`;
   - `_reviews\094-27b-ruling.md` (FI1–FI6);
   - `_reviews\094-27-ruling.md` (FH1–FH9);
   - `_reviews\094-27-request.md` (FB1–FB7);
   - `_reviews\094-26d-ruling.md` (FG1–FG4);
   - `_reviews\094-26c-ruling.md` (FF1–FF5);
   - `_reviews\094-22-ruling.md` (EX3's quarantine);
   - `_reviews\094-27b-code-review.md` and the 094-27b mailbox report.
2. **Reuse your predecessors' tooling** in `code-receipts-094-27b\` (the runtime review) and `code-receipts-094-26d\` (the authoring review), and the older folders they drew on. That includes:
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
- **Check `m51` against P9, and 094-27b's results,** as above. Stop if either fails.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **Record:**
  - the feature HEAD;
  - the 4.25 manifest (27 files, `c501895c…`);
  - the launcher's state;
  - **your own before-lists:** a hash list of the Paris host's whole `Content` folder (with the three current A6 packages' hashes called out), and hashes of its `Config` files, plugin descriptors and `.uproject`;
  - the EditorRuns markers, including 31124;
  - the crash folders: the host's `Saved\Crashes`, the engine's, and `%LOCALAPPDATA%`'s (names only);
  - the drive-root listings of C:, D: and E: (top-level names only).
- **No EK1 pre-scan and no full P9 before-snapshot** (FA6).

### Step 1. The canary and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-27c\canary.txt`. The value goes nowhere else.
- **Preserve 094-27b's report outputs in the run folder** before the resume overwrites them.
- **The resume prompt:** `resume-prompt-094-27c.txt` in the run folder, as before, UTF-8 with no BOM. Its contents, in order:
  1. `_reviews\094-27c-request.md`, verbatim;
  2. clearly marked Code-added context: `m51` status, 094-27b's verdict and HEAD, any GAP your predecessor raised that the ruling doesn't cover, memory, free space, the canary's name only, verified paths, the launcher's state, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm that a new turn started.
  - If the resume fails twice, start a fresh thread, as before.
  - A relay retry after a model-capacity error is not a failure. Record it.

### Step 2. Poll (W3)
- Use light condition loops until `exit-code.txt` appears.
- **Write one compact line per poll** in `code-receipts-094-27c\midjob-notes.txt`, by absolute path. Each line records:
  - free space, memory and commit;
  - the processes of ours (`UE4Editor`, a `-game` UE4Editor, `ShaderCompileWorker`, `CrashReportClient*`), with their PIDs and parents;
  - for any `CrashReportClient*` process, its `-MONITOR=` value.
- **Expected, in order:**
  1. the A6 quarantine;
  2. two authoring sessions;
  3. the runtime kit's 18 legs, one session each.

  The crash monitor names an owned PID (FH5). Record anything else, but never act on it.
- **A new or removed drive-root entry while Codex runs:** gather read-only writer evidence (names, sizes, counts, and the running program that names the path). Never open its files. Record it; don't act (EZ1).
- **Never steer Codex. Never touch the editor, the game, their windows or their dialogs.**

### Step 3. Collect and review (after Codex exits)
**Collect:**
- Codex's report to `_reviews\094-27c-codex-report.md`, with a header line;
- the ledger row.

**Write `_reviews\094-27c-code-review.md`:**
- **The rulings table:** FI1–FI6, FH1–FH9, FG1–FG4, FB1–FB7, FF1–FF5, FE1–FE5, FD1–FD6, FC1–FC9, FA1–FA8, EZ1–EZ6, EY1–EY8, EX1–EX6 and EW1–EW4, with the earlier rulings in one row each where they apply.
- **FI2, yourself:**
  - search every script the two kits run inside the editor or the game for `unreal.Rotator(` with positional arguments: there must be 0;
  - check that the stub's `Rotator` signature is `(roll=0.0, pitch=0.0, yaw=0.0)`;
  - run the old camera line through the harness yourself: it must fail.
- **FI3, yourself:**
  - check Codex's crash-cause citations in the driver source and in 094-27b's Load log;
  - run the harness can-fails yourself (one exit request, no load after it, and the callback unregistered on its first raise).
- **The authoring:**
  - the quarantine receipts for the three old packages;
  - per recipe: gate wait, start, end, duration and exit, a clean `LogExit`, and one `probe_stop`;
  - the in-session camera read-back (pitch −12, yaw 0, roll 0, within ±0.01°);
  - exactly the three declared packages are new, with their sizes and SHA-256s;
  - nothing else under Content changed, from your own after-lists.
- **The runtime kit:**
  - read the diff, and re-run only the suites that cover changed or new code;
  - add your own can-fail for FI4 (a first crash continues, a second ends the run);
  - check that the arming binds the new package hashes.
  - Create no reparse point anywhere (FA1).
- **The rehearsal, dry runs and live readings:** check them from their receipts, and do your own live compare once.
- **The real runtime run, per leg, from the raw files yourself:**
  - the console lines against the kit's catalog;
  - the idle pre-check before each start, and the gate waits;
  - start, end, duration and exit;
  - the probe rows;
  - the Load leg's read-back of every saved value, the camera included;
  - for A7, count the events per anomaly yourself from the probe rows (attempted, valid, applied, reverted, refused, invalid), and compare with the reader;
  - FH4: each leg's evidence hashes;
  - any crash, with its leg (FI4).
- **Each gate's verdict** (A3, A5, A6, A7 and A8, as FB1 scopes them): agree or disagree, with the reason.
- **The pictures:**
  - every screenshot named in the contact-sheet index exists, opens as a 1280×720 PNG, and belongs to the event its row names;
  - its AA echo is 0;
  - the contact sheets exist.
  - Look at one contact sheet yourself, and say whether the camera view looks level and slightly downward.
- **No uploads:**
  - the EU4 and FC4 switches are in every recorded argv, the `-game` legs included;
  - the session and crash-client logs show no data-router POST and no other upload;
  - list any new crash or ensure report folder, with its leg.
- **FB7 and the classes:**
  - the Python 2.7 harness on the changed scripts;
  - the smoke's receipt and its clean exit;
  - every UnrealBuildTool child recorded with its argv and no build action (FD2);
  - FF2's, FG2's and FI5's classes.
- **The EditorRuns markers** per ET2, with 31124 unchanged.
- **The ledger:** C:, E: and 5.1-engine writes inside the EA4, EG1 and ET3 classes, and any foreign root entries recorded under EZ1.
- **The 4.25 manifest is unchanged** (27 files).
- **EU1:** check Codex's command audit, and spot-check it against the event log.
- **EL2:** check Codex's canary scan, and the canary by name in the recorded child environments, the adopted monitors included: 0 of each.
- **P9 v2:** check Codex's EA8 entry and exit proofs. Do no re-hash of your own (FA6).
- **Launches:** exactly the two authoring sessions and the runtime kit's legs, one session each, and nothing else.
- **Where you may write:** `D:\IA_Europa\Receipts\CodeReview09427c\` and `D:\IA_Europa\Temp\CodeReview09427c\`.
- **Your top 5 risks** for Phase B: the six basic anomalies, with 4.25-authored materials.

## Hard rules
- **No source edits, builds, launches, commits or pushes by you.** The only exceptions are suite and can-fail runs on owned scratch, and read-only scans.
- **Never run any kit for real.** Dry runs and rehearsals only, under your own review folders.
- **Never touch the editor, the game, their windows or their dialogs.**
- **Never open or read a file inside a foreign drive-root entry** (EZ1). Names, sizes and counts only.
- **You delete nothing.**
- **Never create a junction, symlink or other reparse point** anywhere under `D:\IA_Europa` or the run folder (FA1).
- **The only writes allowed to you:**
  - `code-receipts-094-27c\`;
  - the resume prompt, and the preserved copies of 094-27b's report outputs;
  - the two `_reviews\094-27c-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09427c\`;
  - `D:\IA_Europa\Temp\CodeReview09427c\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging, power or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs, and never stop the editor, the game or the crash monitor: the kits' own limits do that.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
   - **GREEN:** all of these hold:
     - both maps were re-authored, with the camera check passing;
     - every runtime leg ran;
     - gates A3, A5, A6, A7 and A8 (as FB1 scopes them) pass, by the readers and by your own counts;
     - the contact sheets exist;
     - nothing outside the three declared outputs changed under Content, config, the descriptors or the `.uproject`;
     - no upload, and the canary is absent;
     - there is no blocking GAP;
     - P9 v2 is unchanged.
   - **NEEDS-DECISION:** any of these:
     - steps 2–5 failed before a real launch;
     - there was no idle window;
     - some legs didn't run;
     - a gate failed;
     - one runtime crash (FI4).
   - **RED:** any of these:
     - a crash in an authoring session, or a second runtime crash;
     - a protected-state change;
     - a Content change outside the three declared outputs;
     - an upload.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, the pauses, the retries, any foreign root entries, and its VERDICT line.
5. **The fixes and the run-up,** in 6–8 lines: FI2's static check, FI3's crash cause and can-fails, the rehearsal, both armings, the dry runs and the live readings.
6. **The authoring:** per recipe, the gate wait, start, end, duration, exit and the camera read-back; the new outputs.
7. **The runtime run, per leg:** gate wait, start, end, duration, exit and reader verdict.
8. **The gates:** a table with A3 (read-back), A5, A6, A7 (per anomaly: valid events, applied, reverted) and A8, each with the reader's verdict and yours.
9. **The pictures:** the path of the contact-sheet index, the 4 contact sheets, and your one-sheet look.
10. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
11. **Owner actions:**
    - the eye check: which files to open, and what to look for;
    - anything left on screen.
12. **NEEDS-DECISION items,** or "none".
