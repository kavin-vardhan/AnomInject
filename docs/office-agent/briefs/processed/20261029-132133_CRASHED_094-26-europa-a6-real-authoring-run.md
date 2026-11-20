# ACK-VERDICT: 094-25 NEEDS-DECISION is ruled in _reviews\094-25-ruling.md (FA1-FA7): Codex's GREEN is accepted and the 09425 kit is the one this run uses; FA1 Code removes its own test junction (link only, by receipt) and its own dry run must pass before Codex is launched; FA5 the owner wants speed, so the run starts now and goes whenever the PC is idle, until 06:00 IST 10-30; FA6 lean checks; no tool creates reparse points under D:\IA_Europa.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-26 Europa A6 real fixture-authoring run, starting now whenever the PC is idle: Codex runs the armed 09425 kit, Code checks the evidence after

# 094-26 — The real A6 fixture-authoring run, starting now, whenever the PC is idle (EA_Core, then EA_Travel): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**This job launches the 4.25 editor, but only through the armed kit, and only Codex starts the kit for real.** You never launch anything, touch the editor, or answer its dialogs. Your one kit run is FA1's dry run, on throwaway slots.

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-26\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Launch hold:** launch once D: reads at least 40 GiB (EV3) and E: at least 15 GiB (EY4) on two polls.
- **The window (FA5):** from Codex's start until 06:00 IST on 10-30. Codex starts the kit's entry point only after the PC has been idle for 10 minutes, and the kit's own gate then decides each session. The owner wants speed: launch Codex as soon as FA1 and your start checks pass.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-25 did. Poll with light, bounded condition loops, one at a time: about 10 minutes each, and up to 20 minutes while Codex is only waiting for an idle PC.
- **Never end the run while the Codex job is still running.** The cap is 24 h after launch (FA5); past that, leave it running and report.
- **Keep context lean:** one compact line per poll. This run may wait for hours. Your predecessor ended at 76 % of its context.

## Bootstrap
1. Read in full:
   - `_reviews\094-25-ruling.md`;
   - `_reviews\094-24-ruling.md`, EZ1, EZ2 and EZ5;
   - `_reviews\094-23-ruling.md`, especially EY7;
   - `_reviews\094-26-request.md`;
   - `_reviews\094-25-code-review.md`.
2. Reuse your predecessors' tooling in `code-receipts-094-25\` and `code-receipts-094-24\`: the kit and slot checks, the rehearsal and stand-in sets, the live-gate check, the scanners, the command audit, the `m51` check, the disk and memory poller, and the poll script.

## The work

### Step 0. Start checks
- **Record:** headroom (ED1, with EV3's D: lines and EY4's E: lines), memory (physical and commit), the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check (FA6's quick checks):** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.** No EK1 pre-scan and no full P9 before-snapshot (FA6).
- **Record:**
  - the feature HEAD and the kit's bound commit;
  - the three bound manifests;
  - the launcher's state (it must already be running; the kit never starts it);
  - the 09425 kit's state, with all 15 real paths absent;
  - **your own before-lists:** a hash list of the Paris host's whole `Content` folder, and hashes of its `Config` files, plugin descriptors and `.uproject`;
  - the EditorRuns markers, including 31124.

### Step 0a. FA1: remove your predecessor's test junction, then prove the kit's entry passes
1. **Confirm** that `D:\IA_Europa\Temp\CodeReview09425\ez1-canfails\roots\OwnerWithJunction\link` is a directory junction whose target is `D:\IA_Europa\Temp\CodeReview09425\ez1-canfails\roots\junction-target`. Record the target folder's listing and `safe.txt`'s size and SHA-256. List every reparse point under `D:\IA_Europa\Receipts\CodeReview*` and `D:\IA_Europa\Temp\CodeReview*`: there must be exactly this one.
2. **Remove the link only:** `cmd /d /c rmdir "D:\IA_Europa\Temp\CodeReview09425\ez1-canfails\roots\OwnerWithJunction\link"`. No `/s`, no `/q`, no other delete.
3. **Confirm** the link is gone, and that the target folder and `safe.txt` are unchanged.
4. Write all of it to `code-receipts-094-26\fa1-junction-removal.json`.
5. **Your own dry run of the real entry point,** on throwaway slots under `D:\IA_Europa\Receipts\CodeReview09426\` and `D:\IA_Europa\Temp\CodeReview09426\`, as your predecessors ran it: both recipes must report `armed dry run passed`, with 0 launches and the 15 real paths still absent.
6. **Anything unexpected** (not a junction, another target, a second reparse point, or a failing dry run): stop with `VERDICT: NEEDS-DECISION`. Don't launch Codex.

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-26\canary.txt`, and the value goes nowhere else.
- **Preserve 094-25's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-26.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-26-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, the launcher's state, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- One compact line per poll in `code-receipts-094-26\midjob-notes.txt`, by absolute path: free space, memory, commit, and the processes of ours (`UE4Editor`, `ShaderCompileWorker`, `CrashReportClient`) with their PIDs and parents.
- **Expected:** our editor processes appear only inside the kit's sessions, as children of the kit, and at most one session per recipe. Record anything else; never act on it.
- **A new or removed drive-root entry while Codex runs:** gather read-only writer evidence (names, sizes, counts, and the running program that names the path). Never open its files. Record it; don't act (EZ1).
- **Never steer Codex. Never touch the editor, its windows or its dialogs.**

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-26-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-26-code-review.md`:**
  - **The rulings table:** FA1–FA7, EZ1–EZ6, EY1–EY8, EX1–EX6, EW1–EW4, EV1–EV7, EU1–EU6, ET1–ET7, ER1–ER4, EQ1–EQ5, EP1–EP3, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2, EG1/EG2.
  - **The run, per recipe:** the console lines against the kit's console catalog (each must be a catalogued line); the idle pre-check before each start; gate wait; session start and end; duration; exit code; the attempt slot used. Check the window (FA5) and that any re-run came only after the exact no-idle-window line.
  - **The clean exit, from the raw files yourself:** in `probe.jsonl`, the skipped cleanup row and exactly one `probe_stop`; no PIE. List every error and ensure line in `Runtime.log`, with your own classification: engine or host content, or our plugin or probe.
  - **The outputs, from your own after-lists:** exactly the declared packages are new, with sizes and hashes; every other Content file, the config files, the descriptors and the `.uproject` unchanged; no BuiltData or other undeclared file.
  - **The authored content:** check the authoring receipt against the committed A5 recipe and manifest yourself, item by item.
  - **No uploads:** the EU4 switch is in every recorded argv; the crash-client log and the session logs show no upload. List any crash or ensure report folder, with its files.
  - **The EditorRuns markers** per ET2, with 31124 unchanged.
  - **The ledger:** C:, E: and 5.1-engine writes inside the EA4, EG1 and ET3 classes.
  - **A failed session:** EX3's quarantine receipt, checked against your after-lists.
  - **The bound binaries and manifests are unchanged** (110 of 110).
  - **EU1:** check Codex's command audit; spot-check it against the event log.
  - **EL2:** check Codex's canary scan, and the canary by name in recorded child environments: 0 of each.
  - **P9 v2:** check Codex's EA8 entry and exit proofs (no re-hash of your own, FA6).
  - **Launches:** exactly the kit's sessions, at most one per recipe; nothing else.
  - **No suite re-runs and no full scans of your own** (FA6). You may write under `D:\IA_Europa\Receipts\CodeReview09426\` and `D:\IA_Europa\Temp\CodeReview09426\`.
  - **Your top 5 risks** for A7 (the first 4.25 pictures in these two maps).

## Hard rules
- **No source edits, builds, launches, commits or pushes by you.** The only exceptions are self-test re-runs and read-only scans.
- **Never run any kit for real.** Your only kit run is FA1's dry run, on throwaway slots under your own review folders.
- **Never touch the editor, its windows or its dialogs.**
- **Never open or read a file inside a foreign drive-root entry** (EZ1). Names, sizes and counts only.
- **You delete nothing,** except FA1's one junction link, exactly as Step 0a says.
- **Never create a junction, symlink or other reparse point** anywhere under `D:\IA_Europa` or the run folder (FA1). Reparse can-fails use a synthetic model only.
- **The only writes allowed to you:**
  - `code-receipts-094-26\`;
  - the resume prompt, and the preserved copies of 094-25's report outputs;
  - the two `_reviews\094-26-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09426\`;
  - `D:\IA_Europa\Temp\CodeReview09426\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging, power or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs, and never stop the editor: the kit's own limit does that.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
   - **GREEN:** both recipes completed with clean exits; exactly the three declared outputs are new and match the recipe; everything else is unchanged; no upload; the canary is absent; there is no blocking GAP; P9 v2 is unchanged.
   - **NEEDS-DECISION:** no session started (no idle window), or only `EA_Core` completed.
   - **RED:** a crash, a protected-state change, or an upload.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **FA1 and the Codex job:** the junction removal and your dry run; then run folder, thread, duration, exit, the pauses, the retries, and its VERDICT line.
5. **The run, per recipe:** gate wait, start, end, duration, exit, and the console lines shown.
6. **The evidence** in 6–8 lines: clean exit, outputs, authored content, uploads, markers, ledger.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
8. **Owner actions** for the morning, including anything left on screen.
9. NEEDS-DECISION items, or "none".
