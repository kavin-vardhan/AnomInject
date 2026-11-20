# ACK-VERDICT: 094-23 NEEDS-DECISION is ruled in _reviews\094-23-ruling.md (EY1-EY8): EX1-EX5 accepted; G1 fixed by removing the variable, with an environment-read check and a real-branch rehearsal using a stand-in editor; EX1 narrowed to the launcher's own install folders; O2 60 s stop, UnrealPak and UnrealLightmass block, a 12 GiB free-commit start line; E: lines 15/12/10; the port is 4.25 only (owner); 094-24 re-arms for 09424, dry-run only; the real run is 094-25.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-24 Europa A6 fix G1, narrow the launcher exemption, rehearse the real branch, re-arm for 09424, dry-run only: Codex implements, Code reviews after

# 094-24 — Fix G1, rehearse the real branch with a stand-in editor, and re-arm the A6 fixture-authoring kit for 09424 (no launches): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Tonight depends on this job.** If it ends GREEN, the real authoring run (094-25) starts after midnight. Be thorough, and keep moving.

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-24\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Launch hold:** launch once D: reads at least 40 GiB (EV3) and E: at least 15 GiB (EY4) on two polls.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-23 did. Poll with light, bounded condition loops, one at a time.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- **Keep context lean:** one compact line per poll. Your predecessor ended at 72 % of its context.

## Bootstrap
1. Read in full:
   - `_reviews\094-23-ruling.md`;
   - `_reviews\094-24-request.md`;
   - `_reviews\094-23-code-review.md`.
2. Reuse your predecessor's tooling in `code-receipts-094-23\`:
   - your G1 reproduction through the real `fixture_leg.main()` (`review\marker-env23.json`);
   - the live-compare check and your EX can-fails;
   - the `.bat` dry-run harness, the scanners and the command audit;
   - the `m51` check, the disk and memory poller, and the poll script.

## The work

### Step 0. Start checks
- **Record:** headroom (ED1, with EV3's D: lines and EY4's E: lines), memory (physical and commit), the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **The EK1 pre-scan:** names and counts only.
- **Record:**
  - the feature HEAD;
  - the three bound manifests;
  - the launcher's state;
  - the 09423 kit's state, with all 15 real paths absent;
  - a hash list of the Paris host's `Content`.

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-24\canary.txt`, and the value goes nowhere else.
- **Your own before-snapshots,** as in 094-23, against P9 v2.
- **Preserve 094-23's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-24.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-24-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.
  - A relay retry after a model-capacity error is not a failure. Record it, as you did in 094-23.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- One compact line per poll in `code-receipts-094-24\midjob-notes.txt`, by absolute path, with free space, physical memory and commit. Never steer Codex.
- **No launches.** Watch for any `UE4Editor`, `UnrealEditor`, `ShaderCompileWorker`, `CrashReportClient` or UAT process of ours.

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-24-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-24-code-review.md`:**
  - **The rulings table:** EY1–EY8, EX1–EX6, EW1–EW4, EV1–EV7, EU1–EU6, ET1–ET7, ER1–ER4, EQ1–EQ5, EP1–EP3, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2, EG1/EG2.
  - **EY1, G1:**
    - read the diff;
    - re-run your G1 reproduction through the real `fixture_leg.main()`: the marker hand-off must now work;
    - check the environment-read check, and add your own mutant that re-introduces a filtered name;
    - re-run Codex's rehearsal yourself, then add your own stand-in variants:
      - an extra undeclared file is refused;
      - a missing `probe_stop` is refused;
      - a stand-in that outlives the deadline is stopped with its tree while a sibling survives;
      - a stand-in that spawns a child named like the crash reporter has that child stopped with the tree;
    - confirm every rehearsal write stayed under owned scratch, and that the Paris host and the real slots are untouched.
  - **EY2, the exemption by folder:**
    - repeat your live compare: the 5 overlays and the web helpers exempt by folder, and 0 blockers;
    - your can-fails: an editor-named process under a launcher-like parent with a non-launcher path blocks; an unreadable path is not exempt.
  - **EY3:** the 60 s unreadable-reading stop; `UnrealPak` and `UnrealLightmass` block while the recorded-only images don't; the 12 GiB free-commit start line and its Waiting line.
  - **EY4:** E:'s lines in the kit and the entry checks. Your own `.bat` dry run must now pass its first check when E: is at 15 GiB or more.
  - **The rest of the tooling:** re-run every self-test, and your 094-23 and 094-22 can-fail sets.
  - **The armed scope** equals EW2 exactly; the 09424 real slots are unused, and the 09423 arming refuses.
  - **Your own dry run** of the real entry point, on throwaway slots, with 0 launches.
  - **The bound binaries and manifests are unchanged** (110 of 110).
  - **The Paris `Content` is unchanged,** from your own hash list.
  - **EU1:** your own command audit.
  - **EL2:** the canary, by value, from the launch second, and by name in recorded child environments: 0 of each.
  - **P9 v2, checked independently.**
  - **Launches:** 0.
  - **All self-test suites.** You may write under `D:\IA_Europa\Receipts\CodeReview09424\` and `D:\IA_Europa\Temp\CodeReview09424\`.
  - **Your top 5 risks** for tonight's real run (094-25, under EY7).

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are self-test, can-fail and rehearsal re-runs, and read-only scans.
- **Never run any kit for real.** Dry runs and rehearsals only, under owned scratch and your own review folders.
- **You delete nothing.**
- **The only writes allowed to you:**
  - `code-receipts-094-24\`;
  - the resume prompt, and the preserved copies of 094-23's report outputs;
  - the two `_reviews\094-24-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09424\`;
  - `D:\IA_Europa\Temp\CodeReview09424\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - G1 is fixed, and the real branch is rehearsed with both a passing and a failing stand-in;
   - EY2, EY3 and EY4 are built and proven, including the live gate reading;
   - the kit is re-armed exactly to EW2, with unused 09424 slots, and the 09423 arming refuses;
   - the dry run launched nothing;
   - the bound binaries, manifests and Paris `Content` are unchanged;
   - the canary is absent;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, the pauses, the retries, and its VERDICT line.
5. **EY1–EY4** in 6–8 lines, including the rehearsal and the live gate reading.
6. **The kit** in 6–8 lines, with the slot paths and every line the run can show.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
8. **Owner actions,** including what the PC must look like for tonight's run.
9. NEEDS-DECISION items, or "none".
