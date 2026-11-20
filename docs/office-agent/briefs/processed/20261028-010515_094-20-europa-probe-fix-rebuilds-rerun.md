# ACK-VERDICT: 094-19 NEEDS-DECISION/RED is ruled in _reviews\094-19-ruling.md (EU1-EU6): a direct read-only command is a recorded deviation with an end-of-job command audit, a foreign-caused disk floor breach pauses instead of stopping, the launcher and upload designs are decided for the arming job, and 094-20 re-runs the probe fix and six rebuilds with the disk-pause tooling first.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-20 Europa disk-pause tooling, probe exit-crash fix, six rebuilds: Codex implements, Code reviews after

# 094-20 — Disk-pause tooling, then the probe's exit-crash fix and the six rebuilds (no launches): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-20\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-19 did. Poll with light, bounded condition loops, one at a time.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- Keep context lean.

## Bootstrap
1. Read in full:
   - `_reviews\094-19-ruling.md`;
   - `_reviews\094-20-request.md`;
   - `_reviews\094-18-ruling.md`;
   - `_reviews\094-19-code-review.md`.
2. Reuse your predecessors' tooling:
   - `code-receipts-094-19\`: the scanners, the `m51` check, the disk and memory poller, and the poll script;
   - `code-receipts-094-15\`: the build review, re-hash, EN1 checker and reader known answers.

## The work

### Step 0. Start checks
- **Record:** headroom (ED1), memory, the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **The EK1 pre-scan:** names and counts only.
- **Record the current state:**
  - the 31124 marker;
  - whether the Epic Games Launcher is running, and whether it is the old editor-spawned one or a clean restart;
  - whether the two owner-slot temp files are readable now. If they are, scan them;
  - the three bound manifests (27/39/44).

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-20\canary.txt`, and the value goes nowhere else.
- **Your own before-snapshots,** as in 094-19, against P9 v2.
- **Preserve 094-19's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-20.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-20-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- Compact notes in `code-receipts-094-20\midjob-notes.txt`, by absolute path. Never steer Codex.
- Record free space and memory at each poll, so EU2's pauses can be checked against your own readings.
- **No launches.** Watch for any `UE4Editor`, `UnrealEditor`, `ShaderCompileWorker`, `CrashReportClient` or UAT process of ours.

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-20-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-20-code-review.md`:**
  - **The rulings table:** EU1–EU6, ET1–ET7, ES1–ES5, ER1–ER4, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2–EI4, EG1/EG2.
  - **EU2:**
    - read the diff;
    - re-run its self-tests;
    - add your own can-fails: a foreign drop pauses and resumes; a port-caused drop stops; no recovery ends at the limit; a drop during a build stops only our tree;
    - check every real pause against your poll readings.
  - **EU1:** build the command audit independently from the event log. Classify every direct command; a direct write-capable command is a STOP.
  - **The fix:**
    - read the diff;
    - re-run the static check and both mutants;
    - add your own mutant (`TActorRange` or `StaticFindObject`);
    - confirm one probe source in all three hosts, the `ENGINE_MAJOR_VERSION` use, and no production edit;
    - look for any remaining teardown-unsafe pattern.
  - **The rebuilds:** as in your 094-15 review. Six legs; EO3 receipts; EN1 on every makefile with both engines' MSVC and SDK pinned; EI4; the re-bound manifests re-hashed.
  - **EU5:** check the texture identification against the asset headers yourself.
  - **EL2:** the canary, by value, in new or changed files from the launch second, and by name in recorded child environments: 0 of each.
  - **P9 v2, checked independently:** re-hash, refs, and `m51` at 15 untracked.
  - **Launches:** 0.
  - **All self-test suites.** You may write under `D:\IA_Europa\Receipts\CodeReview09420\` and `D:\IA_Europa\Temp\CodeReview09420\`.
  - **Your top 5 risks** for the arming job and the next runtime leg.

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are self-test and can-fail re-runs and read-only scans.
- **Never run the kit, not even a dry run.**
- **You delete nothing.** Leave the 31124 marker where it is.
- **The only writes allowed to you:**
  - `code-receipts-094-20\`;
  - the resume prompt, and the preserved copies of 094-19's report outputs;
  - the two `_reviews\094-20-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09420\`;
  - `D:\IA_Europa\Temp\CodeReview09420\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security setting. No ETW, no USN, no elevation. Never change paging or any machine setting.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs: Blender, the launcher, or the old kit console.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - EU2 is built and proven;
   - the probe is fixed, with its static check and mutants;
   - the six rebuilds passed;
   - the manifests are re-bound;
   - EU5 is done;
   - the command audit is clean of write-capable direct commands;
   - there were no launches;
   - the canary is absent;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, any pauses, and its VERDICT line.
5. **EU2, the fix and the rebuilds** in 8–10 lines.
6. **EU5 and the command audit** in 3–4 lines.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
8. Owner actions.
9. NEEDS-DECISION items, or "none".
