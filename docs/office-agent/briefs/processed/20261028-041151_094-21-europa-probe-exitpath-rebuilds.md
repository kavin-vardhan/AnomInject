# ACK-VERDICT: 094-20 NEEDS-DECISION is ruled in _reviews\094-20-ruling.md (EV1-EV7): the probe fix and EU2 are accepted, no edits while a pause is open, D: thresholds relaxed to 40/30/25 GiB, this job's EI2 start gate is commit 10 GiB, and 094-21 closes the Stop() exit path, widens the static check, and runs the six rebuilds overnight.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-21 Europa probe exit-path fix, wider static check, six rebuilds overnight: Codex implements, Code reviews after

# 094-21 — Close one more probe exit path, widen the check, and run the six rebuilds (no launches): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-21\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Launch hold:** launch once D: reads at least **40 GiB** (EV3's new start) on two polls. Don't wait for 70.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-20 did. Poll with light, bounded condition loops, one at a time.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- Keep context lean.

## Bootstrap
1. Read in full:
   - `_reviews\094-20-ruling.md`;
   - `_reviews\094-21-request.md`;
   - `_reviews\094-20-code-review.md`.
2. Reuse your predecessors' tooling:
   - `code-receipts-094-20\`: the EU2 can-fail harness, your static-check mutants, the command audit, the disk and memory poller, the scanners, the `m51` check and the poll script;
   - `code-receipts-094-15\`: the build review, re-hash, EN1 checker and reader known answers.

## The work

### Step 0. Start checks
- **Record:** headroom (ED1, with EV3's D: thresholds), memory, the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **The EK1 pre-scan:** names and counts only.
- **Record:**
  - the feature HEAD;
  - the probe source in all three hosts, which should equal the `1722348` fix;
  - the three bound manifests (27/39/44);
  - the 31124 marker;
  - the launcher's state.

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-21\canary.txt`, and the value goes nowhere else.
- **Your own before-snapshots,** as in 094-20, against P9 v2.
- **Preserve 094-20's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-21.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-21-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, and the Step 0 state, including the actual feature HEAD;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- Compact notes in `code-receipts-094-21\midjob-notes.txt`, by absolute path, with free space and memory at each poll. Never steer Codex.
- **No launches.** Watch for any `UE4Editor`, `UnrealEditor`, `ShaderCompileWorker`, `CrashReportClient` or UAT process of ours.

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-21-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-21-code-review.md`:**
  - **The rulings table:** EV1–EV7, EU1–EU6, ET1–ET7, ES1–ES5, ER1–ER4, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2–EI4, EG1/EG2.
  - **EV5:**
    - read the diff;
    - re-run the widened static check and every mutant;
    - add your own mutants for each new pattern, plus your 094-20 set;
    - confirm no UObject access after the exit purge in any path;
    - confirm one probe source in all three hosts, the `ENGINE_MAJOR_VERSION` use, and no production edit.
  - **The rebuilds:** as in your 094-15 review. Six legs; EO3 receipts; EI2 admissions under EV4's gate; EN1 on every makefile with both engines' MSVC and SDK pinned; EI4; the re-bound manifests re-hashed.
  - **EU2 and EV1:** every pause against your poll readings; no edit inside any pause window.
  - **EU1:** your own command audit from the event log.
  - **EL2:** the canary, by value, in new or changed files from the launch second, and by name in recorded child environments: 0 of each.
  - **P9 v2, checked independently:** re-hash, refs, and `m51` at 15 untracked.
  - **The full EK1 scan:** 0 secret values.
  - **Launches:** 0.
  - **All self-test suites.** You may write under `D:\IA_Europa\Receipts\CodeReview09421\` and `D:\IA_Europa\Temp\CodeReview09421\`.
  - **Your top 5 risks** for the arming job and the first automated leg.

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are self-test and can-fail re-runs and read-only scans.
- **Never run the kit, not even a dry run.**
- **You delete nothing.** Leave the 31124 marker where it is.
- **The only writes allowed to you:**
  - `code-receipts-094-21\`;
  - the resume prompt, and the preserved copies of 094-20's report outputs;
  - the two `_reviews\094-21-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09421\`;
  - `D:\IA_Europa\Temp\CodeReview09421\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - EV5 is done, with its mutants;
   - the six rebuilds passed;
   - the manifests are re-bound;
   - EI4 passed;
   - the full EK1 scan is clean;
   - the command audit has no write-capable direct command and no edit inside a pause;
   - there were no launches;
   - the canary is absent;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, the pauses, and its VERDICT line.
5. **EV5 and the rebuilds** in 8–10 lines, with the new binary hashes for the probe.
6. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
7. Owner actions.
8. NEEDS-DECISION items, or "none".
