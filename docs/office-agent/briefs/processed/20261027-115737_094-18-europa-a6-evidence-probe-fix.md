# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-18 Europa A6 owner-leg evidence, probe exit-crash fix and rebuilds: Codex implements, Code reviews after

# 094-18 — Check the A6 owner leg, fix the probe's exit crash, rebuild (no launches): resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-18\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-17 did. Poll with light, bounded condition loops, one at a time.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- Keep context lean.

## Bootstrap
1. Read in full:
   - `_reviews\094-17-owner-leg-ruling.md`;
   - `_reviews\094-18-request.md`;
   - `_reviews\094-15-ruling.md` (EP3);
   - `_reviews\094-17-code-review.md`.
2. Reuse your predecessors' tooling in `code-receipts-094-15\` (builds: re-hash, EN1 checker, reader known answers) and `code-receipts-094-17\` (kit and slot checks, scanners, `m51` check, poll script).

## The work

### Step 0. Start checks
- **Record:** headroom (ED1), memory, the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **The EK1 pre-scan:** names and counts only.
- **The owner leg's footprint, read-only, before Codex touches anything:**
  - the 09417 slot and state folders: the slot used, the launched mark and the exit code;
  - any `refused-attempts` folder;
  - whether any `UE4Editor` or `CrashReportClient` process is still running;
  - the three bound manifests (27/39/44);
  - a hash list of the Paris host's `Content` and the plugin's `Content`, for comparison against their pre-leg records.

### Step 1. The canary, the snapshots and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-18\canary.txt`, and the value goes nowhere else.
- **Your own before-snapshots,** as in 094-17, against P9 v2.
- **Preserve 094-17's report outputs in the run folder** before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-18.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-18-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, and the owner leg's footprint from Step 0;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- Compact notes in `code-receipts-094-18\midjob-notes.txt`, by absolute path. Never steer Codex.
- **No launches.** Watch for any `UE4Editor`, `UnrealEditor`, `ShaderCompileWorker`, `CrashReportClient` or UAT process of ours.

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-18-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-18-code-review.md`:**
  - **The rulings table:** ES1–ES5, EP3, ER1–ER4, EQ1–EQ5, EO1–EO6, EN1–EN5, EM2, EL1–EL5, EK1–EK3, EI2–EI4, EG1/EG2.
  - **The evidence, checked independently:**
    - the editor log's content-admission lines against the prediction (the five 5.1 packages rejected at the header, and skipped by the registry);
    - prompts; DDC and shader routing;
    - Paris and plugin `Content` unchanged, from your own hash list;
    - the crash files' location, upload or not, and restart or not;
    - C: and E: writes against EA4;
    - the engine trees;
    - P9 v2 and the protected state.
  - **The probe observations:** what was written before the crash, and which A6 gates it meets.
  - **The root cause:** read the probe source at the crash lines yourself, plus 4.25 `EngineUtils.h:248` and the 5.1 equivalent.
  - **The fix:**
    - read the diff;
    - check the static check and its mutant;
    - confirm one probe source in all three hosts, with no production edit;
    - look for any remaining teardown-unsafe pattern.
  - **The rebuilds:** as in your 094-15 review. Six legs; EO3 receipts; EN1 on every makefile with both engines' MSVC and SDK pinned; EI4; re-bound manifests re-hashed.
  - **EL2:** the canary, by value, in new or changed files from the launch second, and by name in child environments: 0 of each.
  - **Launches:** 0.
  - **All self-test suites.** You may write under `D:\IA_Europa\Receipts\CodeReview09418\` and `D:\IA_Europa\Temp\CodeReview09418\`.
  - **Your top 5 risks** for the next runtime leg.

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are self-test and can-fail re-runs and read-only scans.
- **Never run the kit, not even a dry run.** Read the owner leg's folders read-only.
- **You delete nothing.**
- **The only writes allowed to you:**
  - `code-receipts-094-18\`;
  - the resume prompt, and the preserved copies of 094-17's report outputs;
  - the two `_reviews\094-18-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09418\`;
  - `D:\IA_Europa\Temp\CodeReview09418\`.
- **Never print, copy or quote a secret value or the canary value.** The crash window showed account identifiers; leave them out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - the evidence check is complete, with no protected-state difference and no content change;
   - the probe is fixed, with its can-fail;
   - the six rebuilds passed;
   - the manifests are re-bound;
   - there were no launches;
   - the canary is absent;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines).
3. **Start state,** including the owner leg's footprint.
4. **The Codex job:** run folder, thread, duration, exit, and its VERDICT line.
5. **The owner leg's evidence** in 8–10 lines. Cover content admission, the crash-report facts and the A6 gates met.
6. **The fix and the rebuilds** in 6–8 lines.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
8. Owner actions.
9. NEEDS-DECISION items, or "none".
