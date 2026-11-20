# ACK-VERDICT: 094-18 NEEDS-DECISION is ruled in _reviews\094-18-ruling.md (ET1-ET7): A6's owner half is accepted, the 4.25 EditorRuns marker gets a narrow receipted bookkeeping class and stays in place, the launcher's writes are foreign for this leg, and 094-19 fixes the probe (actor iterators and GC-time lookups), rebuilds, and researches the launcher spawn, the uploads and the texture ensure.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-19 Europa probe exit-crash fix, six rebuilds, launcher and upload research: Codex implements, Code reviews after

# 094-19 — Fix the probe's exit crash, rebuild, and research the launcher and uploads (no launches): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-19\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-18 did. Poll with light, bounded condition loops, one at a time.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- Keep context lean.

## Bootstrap
1. Read in full:
   - `_reviews\094-18-ruling.md`;
   - `_reviews\094-19-request.md`;
   - `_reviews\094-17-owner-leg-ruling.md`;
   - `_reviews\094-18-code-review.md`.
2. Reuse your predecessors' tooling:
   - `code-receipts-094-18\`: the owner-leg footprint, the content hash lists, the scanners, the `m51` check and the poll script;
   - `code-receipts-094-15\`: the build review, re-hash, EN1 checker and reader known answers.

## The work

### Step 0. Start checks
- **Record:** headroom (ED1), memory, the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **The EK1 pre-scan:** names and counts only.
- **Record the current state:**
  - the 31124 marker (present, 0 bytes);
  - whether the Epic Games Launcher and the old kit console are still running;
  - whether the two owner-slot temp files are readable now;
  - the three bound manifests (27/39/44).

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-19\canary.txt`, and the value goes nowhere else.
- **Your own before-snapshots,** as in 094-18, against P9 v2.
- **Preserve 094-18's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-19.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-19-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- Compact notes in `code-receipts-094-19\midjob-notes.txt`, by absolute path. Never steer Codex.
- **No launches.** Watch for any `UE4Editor`, `UnrealEditor`, `ShaderCompileWorker`, `CrashReportClient` or UAT process of ours.

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-19-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-19-code-review.md`:**
  - **The rulings table:** ET1–ET7, ES1–ES5, ER1–ER4, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2–EI4, EG1/EG2.
  - **The fix:**
    - read the diff;
    - re-run the static check, and both mutants (an actor iterator and a `FindObject` put back into a teardown path);
    - add your own mutant, such as `TActorRange` or `StaticFindObject`;
    - confirm one probe source in all three hosts, the `ENGINE_MAJOR_VERSION` use, and no production edit;
    - look for any remaining teardown-unsafe pattern.
  - **The rebuilds:** as in your 094-15 review. Six legs; EO3 receipts; EN1 on every makefile with both engines' MSVC and SDK pinned; EI4; the re-bound manifests re-hashed; probe compiles first time or within the per-host fix rule.
  - **ET2:** the marker's receipt matches your Step 0 reading.
  - **ET7:** your own read of the two temp files if released.
  - **The research (ET3, ET4, ET6):**
    - check that each citation is real in source;
    - check that each proposal changes no machine-wide or C: setting;
    - say which proposal you'd pick, and why.
  - **EL2:** the canary, by value, in new or changed files from the launch second, and by name in child environments: 0 of each.
  - **P9 v2, checked independently:** re-hash, refs, and `m51` at 15 untracked.
  - **Launches:** 0.
  - **All self-test suites.** You may write under `D:\IA_Europa\Receipts\CodeReview09419\` and `D:\IA_Europa\Temp\CodeReview09419\`.
  - **Your top 5 risks** for the next runtime leg.

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are self-test and can-fail re-runs and read-only scans.
- **Never run the kit, not even a dry run.**
- **You delete nothing.** Leave the 31124 marker where it is.
- **The only writes allowed to you:**
  - `code-receipts-094-19\`;
  - the resume prompt, and the preserved copies of 094-18's report outputs;
  - the two `_reviews\094-19-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09419\`;
  - `D:\IA_Europa\Temp\CodeReview09419\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the Epic Games Launcher or the old kit console; that is the owner's to do.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - the probe is fixed, with its static check and mutants;
   - the six rebuilds passed;
   - the manifests are re-bound;
   - the research is complete, with citations;
   - there were no launches;
   - the canary is absent;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, and its VERDICT line.
5. **The fix and the rebuilds** in 6–8 lines.
6. **The research** in 6–8 lines: the launcher, the uploads and the texture, each with the proposal you'd pick.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
8. Owner actions.
9. NEEDS-DECISION items, or "none".
