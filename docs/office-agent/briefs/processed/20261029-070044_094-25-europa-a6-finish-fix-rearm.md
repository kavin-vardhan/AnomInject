# ACK-VERDICT: 094-24 NEEDS-DECISION is ruled in _reviews\094-24-ruling.md (EZ1-EZ6): the stop was a false alarm on the owner's OBS folder; ED5's root rule is attributed for new and removed entries; a start-line miss at admission waits; the retained tooling diff is committed as-is first; G2 every retained suite must pass before the re-arm; 094-25 finishes, rehearses on the final commit and re-arms for 09425, dry-run only; the real run is 094-26 tomorrow night.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-25 Europa A6 finish the G1 fix, attribute root folders, complete the rehearsal, re-arm for 09425, dry-run only: Codex implements, Code reviews after

# 094-25 — Finish 094-24: commit the retained tooling, attribute drive-root folders, complete the real-branch rehearsal, and re-arm the A6 fixture-authoring kit for 09425 (no launches): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**The owner may use the PC tonight** (OBS recording to E:, Steam, Blender). Foreign disk and memory swings are expected; record them, and never act on the owner's programs.

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-25\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Before launching Codex, also check the port worktree's retained diff** (EZ3): exactly 094-24's 15 uncommitted tooling files (11 modified, 4 new), with the hashes in `D:\IA_Europa\Receipts\09424\stop-closeout.json`, on `97de9d4`, and nothing else uncommitted. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Launch hold:** launch once D: reads at least 40 GiB (EV3) and E: at least 15 GiB (EY4) on two polls.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-24 did. Poll with light, bounded condition loops, one at a time.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- **Keep context lean:** one compact line per poll.

## Bootstrap
1. Read in full:
   - `_reviews\094-24-ruling.md`;
   - `_reviews\094-25-request.md`;
   - `_reviews\094-24-code-review.md`;
   - `_reviews\094-23-ruling.md`, EY1–EY4 and EY7.
2. Reuse your predecessors' tooling in `code-receipts-094-24\` and `code-receipts-094-23\`:
   - the G1 reproduction, the rehearsal and stand-in variants, and the live-compare check;
   - the OBS writer evidence (`review\obsvideos-writer.txt`) as your model for read-only writer evidence;
   - the `.bat` dry-run harness, the scanners and the command audit;
   - the `m51` check, the disk and memory poller, and the poll script.

## The work

### Step 0. Start checks
- **Record:** headroom (ED1, with EV3's D: lines and EY4's E: lines), memory (physical and commit), the toolchains (ED3) and the watcher.
- **`m51` against P9, and the retained diff:** as above. Stop if either differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **The EK1 pre-scan:** names and counts only.
- **Record:**
  - the feature HEAD;
  - the three bound manifests;
  - the launcher's state;
  - the 09423 kit's state, with all 15 real paths absent;
  - a hash list of the Paris host's `Content`;
  - the drive-root listings of C:, D: and E: (top-level names only).

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-25\canary.txt`, and the value goes nowhere else.
- **Your own before-snapshots,** as in 094-24, against P9 v2.
- **Preserve 094-24's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-25.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-25-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, the retained-diff check, memory, free space, the canary's name only, verified paths, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.
  - A relay retry after a model-capacity error is not a failure. Record it.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- One compact line per poll in `code-receipts-094-25\midjob-notes.txt`, by absolute path, with free space, physical memory and commit. Never steer Codex.
- **A new drive-root entry while Codex runs:** gather read-only writer evidence, as you did for OBS (names, sizes, counts, and the running program that names the path). Never open its files. Record it; don't act.
- **No launches.** Watch for any `UE4Editor`, `UnrealEditor`, `ShaderCompileWorker`, `CrashReportClient` or UAT process of ours.

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-25-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-25-code-review.md`:**
  - **The rulings table:** EZ1–EZ6, EY1–EY8, EX1–EX6, EW1–EW4, EV1–EV7, EU1–EU6, ET1–ET7, ER1–ER4, EQ1–EQ5, EP1–EP3, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2, EG1/EG2.
  - **EZ3:** the first commit equals the 15 hashed files exactly; EZ1, EZ2, G2, O1 and O2 are in their own commits.
  - **EZ1, the attributed root rule:**
    - read the diff and Codex's port-pattern list;
    - your own can-fails: a new root folder holding only foreign-pattern files is recorded and the job continues; one holding a port-pattern file is a STOP; one named in a port process's command line is a STOP; a new entry beside `D:\IA_Europa` stays port-attributed; a removed foreign root folder is recorded; a removed P9-protected root is RED;
    - check that no file inside a foreign entry was ever opened.
  - **EZ2:** a drive between its floor and its start line waits at admission with its own Waiting line and passes when it recovers; below a floor, EU2's pause; EY4's lines unchanged.
  - **EZ6:**
    - **G2:** the 09417 EQ suite and every other retained suite pass in full on the final commit, and each updated synthetic reading keeps its case's intent. Update your own older can-fail sets the same way (commit reading and image paths), then re-run them;
    - **O1:** the environment-read check now catches your four copy-based forms;
    - **O2:** the rehearsal console's deadline wording;
    - **O3:** any timing test that failed under load passed on one quiet re-run.
  - **EY1, the rehearsal:** check Codex's full set on the final commit, then re-run your own five stand-in variants on it:
    - an extra undeclared file is refused;
    - a missing `probe_stop` is refused;
    - a stand-in that outlives the deadline is stopped with its tree while a sibling survives;
    - a crash-reporter-named child is stopped with the tree;
    - every rehearsal write stayed under owned scratch; the Paris host and the real slots are untouched.
  - **EY2:** repeat your live compare: the 5 overlays and the web helpers exempt by folder, and 0 blockers. Your 094-24 can-fails again.
  - **EY3:** the 60 s unreadable-reading stop; the blocking and recorded-only images; the 12 GiB free-commit start line.
  - **The rest of the tooling:** re-run every self-test, and your 094-24, 094-23 and 094-22 can-fail sets.
  - **The armed scope** equals EW2 exactly; the 09425 real slots are unused, and the 09423 arming refuses.
  - **Your own dry run** of the real entry point, on throwaway slots, with 0 launches.
  - **The bound binaries and manifests are unchanged** (110 of 110).
  - **The Paris `Content` is unchanged,** from your own hash list.
  - **EU1:** your own command audit.
  - **EL2:** the canary, by value, from the launch second, and by name in recorded child environments: 0 of each.
  - **P9 v2, checked independently.**
  - **Launches:** 0.
  - **All self-test suites.** You may write under `D:\IA_Europa\Receipts\CodeReview09425\` and `D:\IA_Europa\Temp\CodeReview09425\`.
  - **Your top 5 risks** for the real run (094-26, under 094-23 EY7 and 094-24 EZ5).

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are self-test, can-fail and rehearsal re-runs, and read-only scans.
- **Never run any kit for real.** Dry runs and rehearsals only, under owned scratch and your own review folders.
- **Never open or read a file inside a foreign drive-root entry** (EZ1). Names, sizes and counts only.
- **You delete nothing.**
- **The only writes allowed to you:**
  - `code-receipts-094-25\`;
  - the resume prompt, and the preserved copies of 094-24's report outputs;
  - the two `_reviews\094-25-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09425\`;
  - `D:\IA_Europa\Temp\CodeReview09425\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - the retained diff is committed exactly, EZ1 and EZ2 are built and proven, and every retained suite passes (G2);
   - the real branch is rehearsed on the final commit, with every case passing;
   - EY2's live gate reading is clean;
   - the kit is re-armed exactly to EW2, with unused 09425 slots, and the 09423 arming refuses;
   - the dry run launched nothing;
   - the bound binaries, manifests and Paris `Content` are unchanged;
   - the canary is absent;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, the pauses, the retries, any foreign root entries, and its VERDICT line.
5. **EZ1–EZ3 and the rehearsal** in 6–8 lines, including the live gate reading.
6. **The kit** in 6–8 lines, with the slot paths and every line the run can show.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
8. **Owner actions,** including what the PC must look like for the real run.
9. NEEDS-DECISION items, or "none".
