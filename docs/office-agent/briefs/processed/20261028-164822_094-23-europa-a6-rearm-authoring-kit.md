# ACK-VERDICT: 094-22 NEEDS-DECISION is ruled in _reviews\094-22-ruling.md (EX1-EX6): the design is accepted, the launcher's whole process tree is excluded from the foreign-Unreal check with a live gate reading in the dry run, the in-session rules are disk floors, the commit stop and the time limit only, partial outputs are quarantined by receipt, and 094-23 re-arms the kit, dry-run only.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-23 Europa A6 fix the launcher gate and in-session rules, re-arm the authoring kit, dry-run only: Codex implements, Code reviews after

# 094-23 — Fix the launcher gate and the in-session rules, then re-arm the A6 fixture-authoring kit (no launches): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-23\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Launch hold:** launch once D: reads at least 40 GiB (EV3) on two polls.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-22 did. Poll with light, bounded condition loops, one at a time.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- Keep context lean.

## Bootstrap
1. Read in full:
   - `_reviews\094-22-ruling.md`;
   - `_reviews\094-23-request.md`;
   - `_reviews\094-22-code-review.md`.
2. Reuse your predecessor's tooling in `code-receipts-094-22\`: your G4 live-gate check, your can-fail set, the `.bat` dry-run harness, the scanners, the command audit, the `m51` check, the disk and memory poller, and the poll script.

## The work

### Step 0. Start checks
- **Record:** headroom (ED1, with EV3's D: thresholds), memory, the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **The EK1 pre-scan:** names and counts only.
- **Record:**
  - the feature HEAD;
  - the three bound manifests;
  - the launcher's state, and whether the PC was restarted (the uptime);
  - the 09422 kit's state, with all 15 real paths absent;
  - a hash list of the Paris host's `Content`.

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-23\canary.txt`, and the value goes nowhere else.
- **Your own before-snapshots,** as in 094-22, against P9 v2.
- **Preserve 094-22's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-23.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-23-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- Compact notes in `code-receipts-094-23\midjob-notes.txt`, by absolute path, with free space and memory at each poll. Never steer Codex.
- **No launches.** Watch for any `UE4Editor`, `UnrealEditor`, `ShaderCompileWorker`, `CrashReportClient` or UAT process of ours.

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-23-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-23-code-review.md`:**
  - **The rulings table:** EX1–EX6, EW1–EW4, EV1–EV7, EU1–EU6, ET1–ET7, ER1–ER4, EQ1–EQ5, EP1–EP3, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2, EG1/EG2.
  - **EX1, the launcher tree:**
    - read the diff;
    - re-run your 094-22 G4 live check against the real running launcher: its overlay children must now be excluded and recorded;
    - add your own can-fails: a foreign Unreal or game process outside the launcher tree still blocks; a process whose parent is the launcher but whose own children are foreign Unreal is judged by ancestry each poll; the launcher absent means the kit waits;
    - check Codex's live, observe-only gate reading against your own reading at the same time.
  - **EX2, the in-session rules:**
    - our own editor's memory and GPU use never stop the session;
    - free commit below 6 GiB stops only our tree;
    - a disk floor breach stops only our tree;
    - the 1,800 s limit stops only our tree, crash reporter included;
    - human input and foreign processes are recorded, not acted on.
  - **EX3, the quarantine:** a synthetic partial output at a declared path is moved by receipt into the attempt's quarantine folder; nothing outside the declared paths changes; a retry still refuses without a re-arm.
  - **EX5:** the job-template wording says "armed for EW2".
  - **The rest of the tooling:** re-run every self-test and your 094-22 can-fail set.
  - **The armed scope** equals EW2 exactly; the 09423 real slots are unused, and the 09422 arming refuses.
  - **Your own dry run** of the real entry point, on throwaway slots, with 0 launches.
  - **The bound binaries and manifests are unchanged,** and the kit is bound to EW1's manifests (`c501895c…`, `b05e6f8c…`, `3865fe0c…`).
  - **The Paris `Content` is unchanged,** from your own hash list.
  - **EU1:** your own command audit.
  - **EL2:** the canary, by value, from the launch second, and by name in recorded child environments: 0 of each.
  - **P9 v2, checked independently.**
  - **Launches:** 0.
  - **All self-test suites.** You may write under `D:\IA_Europa\Receipts\CodeReview09423\` and `D:\IA_Europa\Temp\CodeReview09423\`.
  - **Your top 5 risks** for the first automated run.

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are self-test and can-fail re-runs and read-only scans.
- **Never run any kit for real.** Dry runs only, on throwaway slots under your own review folders.
- **You delete nothing.**
- **The only writes allowed to you:**
  - `code-receipts-094-23\`;
  - the resume prompt, and the preserved copies of 094-22's report outputs;
  - the two `_reviews\094-23-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09423\`;
  - `D:\IA_Europa\Temp\CodeReview09423\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - EX1, EX2, EX3 and EX5 are built and proven, including the live gate reading;
   - the kit is re-armed exactly to EW2, with unused slots, and the 09422 arming refuses;
   - the dry run launched nothing;
   - the bound binaries, manifests and Paris `Content` are unchanged;
   - the canary is absent;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, the pauses, and its VERDICT line.
5. **EX1–EX5** in 6–8 lines, including the live gate reading.
6. **The kit** in 6–8 lines, with the slot paths and every line the run can show.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
8. Owner actions, including what the PC must look like for the real run.
9. NEEDS-DECISION items, or "none".
