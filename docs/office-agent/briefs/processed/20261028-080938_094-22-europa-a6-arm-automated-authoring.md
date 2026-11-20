# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-22 Europa A6 arm the automated fixture-authoring leg, dry-run only: Codex implements, Code reviews after

# 094-22 — Arm the automated A6 fixture-authoring leg (no launches): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-22\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Launch hold:** launch once D: reads at least 40 GiB (EV3) on two polls.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-21 did. Poll with light, bounded condition loops, one at a time.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- Keep context lean.

## Bootstrap
1. Read in full:
   - `_reviews\094-21-ruling.md`;
   - `_reviews\094-22-request.md`;
   - `_reviews\094-21-code-review.md`;
   - `_reviews\094-17-code-review.md`, the owner-kit sections.
2. Reuse your predecessors' tooling:
   - `code-receipts-094-17\`: the kit and slot checks, the collector stress test, the post-Popen reproduction, and the `.bat` dry-run harness;
   - `code-receipts-094-21\`: the scanners, the command audit, the `m51` check, the disk and memory poller, and the poll script.

## The work

### Step 0. Start checks
- **Record:** headroom (ED1, with EV3's D: thresholds), memory, the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **The EK1 pre-scan:** names and counts only.
- **Record:**
  - the feature HEAD;
  - the three bound manifests;
  - the launcher's state;
  - a hash list of the Paris host's `Content`.

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-22\canary.txt`, and the value goes nowhere else.
- **Your own before-snapshots,** as in 094-21, against P9 v2.
- **Preserve 094-21's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-22.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-22-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- Compact notes in `code-receipts-094-22\midjob-notes.txt`, by absolute path, with free space and memory at each poll. Never steer Codex.
- **No launches.** Watch for any `UE4Editor`, `UnrealEditor`, `ShaderCompileWorker`, `CrashReportClient` or UAT process of ours.

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-22-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-22-code-review.md`:**
  - **The rulings table:** EW1–EW4, EV1–EV7, EU1–EU6, ET1–ET7, ER1–ER4, EQ1–EQ5, EP1–EP3, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2, EG1/EG2.
  - **The design note:**
    - check every 4.25 source citation yourself, including the `-unattended` decision against EW3's three conditions;
    - confirm the mechanism needs no `.uproject` or config change;
    - check that the declared outputs are exact paths inside the fixture folder, and that nothing is overwritten.
  - **The tooling:**
    - read the diff;
    - re-run every self-test;
    - add your own can-fails: launcher absent, so the kit waits; launcher processes are recorded and don't hold the gate; an argv without the EU4 switch refuses; a refusal shows its own reason; a bare `UnrealBuildTool.exe` and a dotnet UBT hold the gate; 8.3 and junction aliases are refused; the time limit stops only the owned tree, crash reporter included; the launched lock refuses after one session per recipe;
    - G1: no `EUROPA_BOOTSTRAP_PROOF` or other wrapper metadata in any recorded child environment, and EN1's strict byte test passes on a synthetic makefile built from the new child environment;
    - O2: your own mutants for the 8 related calls you listed in 094-21.
  - **The armed scope** equals EW2 exactly, and the real slots are unused.
  - **Your own dry run** of the real entry point, on throwaway slots, with 0 launches.
  - **The bound binaries and manifests are unchanged,** and the kit is bound to EW1's manifests (`c501895c…`, `b05e6f8c…`, `3865fe0c…`).
  - **The Paris `Content` is unchanged,** from your own hash list.
  - **EU1:** your own command audit.
  - **EL2:** the canary, by value, from the launch second, and by name in recorded child environments: 0 of each.
  - **P9 v2, checked independently.**
  - **Launches:** 0.
  - **All self-test suites.** You may write under `D:\IA_Europa\Receipts\CodeReview09422\` and `D:\IA_Europa\Temp\CodeReview09422\`.
  - **Your top 5 risks** for the first automated run.

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are self-test and can-fail re-runs and read-only scans.
- **Never run any kit for real.** Dry runs only, on throwaway slots under your own review folders.
- **You delete nothing.**
- **The only writes allowed to you:**
  - `code-receipts-094-22\`;
  - the resume prompt, and the preserved copies of 094-21's report outputs;
  - the two `_reviews\094-22-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09422\`;
  - `D:\IA_Europa\Temp\CodeReview09422\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - the design note is complete, with citations;
   - the kit is armed exactly to EW2, with unused slots;
   - the dry run launched nothing;
   - the bound binaries, manifests and Paris `Content` are unchanged;
   - the canary is absent;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, the pauses, and its VERDICT line.
5. **The design note** in 6–8 lines: the mechanism, the outputs, `-unattended` or not, and the time limit.
6. **The kit** in 6–8 lines, with the slot paths and every line the run can show.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
8. Owner actions, including what the PC must look like for the real run.
9. NEEDS-DECISION items, or "none".
