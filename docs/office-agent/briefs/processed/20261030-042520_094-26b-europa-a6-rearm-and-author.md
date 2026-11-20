# ACK-VERDICT: 094-28 is ruled in _reviews\094-28-ruling.md (FD1-FD6): FC2, FC4 and FC5 hold; our own FC6 guard stopped the smoke session on the editor's stock ValidatePlatforms query, so FD2 judges the UBT child by behaviour; FD3 lowers the memory lines to fit this PC; 094-26b re-arms without dry runs and runs smoke, EA_Core and EA_Travel before 07:30 IST; 094-27 waits behind it.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-26b Europa A6 fix the UBT guard and memory lines, re-arm, then smoke and author EA_Core and EA_Travel for real before morning: Codex runs, Code reviews after

# 094-26b — Fix the UBT guard and the memory lines, re-arm, then the smoke session and the real A6 authoring run before morning: resume Codex, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**This job launches the 4.25 editor, but only through the armed kit, and only Codex starts the kit.** You never launch anything, touch the editor, or answer its dialogs.

## Your working folder is protected. Read this first.
**The watcher starts you inside `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`,** the protected `m51` checkout (P9).
- **Every write you make uses an absolute path** inside your allowed folders. Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-26b\`.
- **Background commands fail closed.** If a `cd` or `Set-Location` fails, nothing after it may write.
- **Before launching Codex,** check `m51` against P9's record with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, and the 15 untracked entries. **Any difference means you stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
- **Launch hold:** launch once D: reads at least 40 GiB (EV3) and E: at least 15 GiB (EY4) on two polls.
- **Launch Codex as soon as your start checks pass.** The owner wants speed (FA5). Codex's real run waits for an idle PC by itself.

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch Codex **detached**, as 094-28 did. Poll with light, bounded condition loops, one at a time: about 10 minutes each, and up to 20 minutes while Codex is only waiting for an idle PC.
- **Never end the run while the Codex job is still running.** The cap is 24 h after launch; past that, leave it running and report.
- **Keep context lean:** one compact line per poll.

## Bootstrap
1. Read in full:
   - `_reviews\094-28-ruling.md`;
   - `_reviews\094-26b-request.md`;
   - `_reviews\094-28-code-review.md` and the 094-28 mailbox report.
   **If 094-28's report or review shows a protected-state change, a crash or an upload, stop with `VERDICT: NEEDS-DECISION`; you do not launch.**
2. Reuse your predecessors' tooling in `code-receipts-094-28\`, `code-receipts-094-26r\` and `code-receipts-094-25\`, including the Python 2.7 harness: the poll and before-list scripts, the live-gate compare, the command-audit check, the `m51` check, and the disk and memory poller.

## The work

### Step 0. Start checks (FA6, lean)
- **Record:** headroom (ED1, with EV3's D: lines and EY4's E: lines), memory (physical and commit), the toolchains (ED3) and the watcher.
- **`m51` against P9:** as above. Stop if it differs.
- **The protected-state check:** StackOBot Content, BankOverflow, the release baseline's 8 hashes, and the cache's `StackOBot\Binaries`. **Any loss is RED: stop.**
- **Record:**
  - the feature HEAD (`dce872d`) and the kit's tooling commit;
  - the 4.25 manifest (27 files, `c501895c…`);
  - the launcher's state;
  - **your own before-lists:** a hash list of the Paris host's whole `Content` folder, and hashes of its `Config\` files, plugin descriptors and `.uproject`, plus a listing of `Saved\` and `Intermediate\` (names, sizes, times) for FC5's classes;
  - the EditorRuns markers, including 31124;
  - the drive-root listings of C:, D: and E: (top-level names only).
- **No EK1 pre-scan and no full P9 before-snapshot** (FA6).

### Step 1. The canary and the resume
- **A fresh `IA_CANARY_TOKEN`:** its name and the value's SHA-256 go in `code-receipts-094-26b\canary.txt`, and the value goes nowhere else.
- **Preserve the run folder's current report outputs** (094-28's) before the resume overwrites them.
- **Resume prompt** `resume-prompt-094-26b.txt` in the run folder, as before, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-26b-request.md` verbatim;
  2. clearly marked Code-added context: `m51` status, memory, free space, the canary's name only, verified paths, the launcher's state, and the Step 0 state;
  3. the GDP implementer block.
- **Resume thread `01a14334`** (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4 and the canary applied.
  - Confirm a new turn started.
  - If the resume fails twice: start a fresh thread, as before.
  - A relay retry after a model-capacity error is not a failure. Record it.

### Step 2. Poll (W3)
- Light condition loops until `exit-code.txt` appears.
- One compact line per poll in `code-receipts-094-26b\midjob-notes.txt`, by absolute path: free space, memory, commit, and the processes of ours (`UE4Editor`, `UnrealBuildTool`, `ShaderCompileWorker`, `CrashReportClient*`) with their PIDs, parents and, for UnrealBuildTool, its command line.
- **Expected:** our editor processes appear only inside the kit's sessions, as children of the kit: the smoke session first, then at most one session per recipe. Record anything else; never act on it.
- **A new or removed drive-root entry while Codex runs:** gather read-only writer evidence (names, sizes, counts, and the running program that names the path). Never open its files. Record it; don't act (EZ1).
- **Never steer Codex. Never touch the editor, its windows or its dialogs.**

### Step 3. Collect and review (after Codex exits)
- **Collect:** Codex's report to `_reviews\094-26b-codex-report.md`, with a header line; the ledger row.
- **Write `_reviews\094-26b-code-review.md`:**
  - **The rulings table:** FD1–FD6, FC1–FC9, FA1–FA8, EZ1–EZ6, EY1–EY8, EX1–EX6, EW1–EW4, and the earlier rulings in one row each where they apply.
  - **FD2:** read the guard change; your own can-fails: `ValidatePlatforms` and `QueryTargets` pass; a build triple, `-Mode=Build`, a compiler child and a write to `Binaries` each stop. Every UBT child in the real run recorded with its argv.
  - **FD3:** the memory lines in the start gate (8 GiB commit, 6 GiB physical) and the in-session stop (4 GiB).
  - **FC2, in the real editor at last:** the smoke receipt shows the launcher loaded the fixture module and every recipe call exists in 4.25.
  - **FC4:** check each analytics switch's 4.25 source citation yourself; then, from the smoke session's and each recipe's `Runtime.log` and `CrashClient.log`, confirm there is no data-router POST and no other upload.
  - **FC5 and FC6:** the managed classes match the rule; every UnrealBuildTool child is recorded with its argv, and no compiler, linker or UHT child ran, and no bound binary or build output changed.
  - **The rehearsal, re-arm and live reading:** check them from their receipts (no dry runs, FD5); do your own live compare once. Create no reparse point anywhere (FA1).
  - **The smoke session, from its raw files:** its receipt (every recipe call present in 4.25), its clean exit (probe rows), nothing created or saved.
  - **The real run, per recipe, from the raw files yourself:** the console lines against the kit's catalog; the idle pre-check before each start; gate waits; start, end, duration, exit; in `probe.jsonl`, the skipped cleanup row and exactly one `probe_stop`, no PIE; every error and ensure line in `Runtime.log`, with your own classification.
  - **The outputs, from your own after-lists:** exactly the three declared packages are new, with sizes and hashes; everything else under Content, `Config\`, the descriptors and the `.uproject` unchanged; changes in FC5's classes listed.
  - **The authored content:** check the authoring receipt against the committed A5 recipe and manifest yourself, item by item.
  - **The EditorRuns markers** per ET2, with 31124 unchanged.
  - **The ledger:** C:, E: and 5.1-engine writes inside the EA4, EG1 and ET3 classes; foreign root entries recorded under EZ1.
  - **The 4.25 manifest and the bound binaries are unchanged.**
  - **EU1:** check Codex's command audit; spot-check it against the event log.
  - **EL2:** check Codex's canary scan, and the canary by name in recorded child environments: 0 of each.
  - **P9 v2:** check Codex's EA8 entry and exit proofs (no re-hash of your own, FA6).
  - **Launches:** the smoke session and at most one session per recipe; nothing else.
  - You may write under `D:\IA_Europa\Receipts\CodeReview09426b\` and `D:\IA_Europa\Temp\CodeReview09426b\`.
  - **Your top 5 risks** for 094-27 (the runtime legs and A7's pictures, which also run in-editor Python and a `-game` leg).

## Hard rules
- **No source edits, builds, launches, commits or pushes by you.** The only exceptions are suite, harness and can-fail runs on owned scratch, and read-only scans.
- **Never run any kit for real.** Dry runs and rehearsals only, under your own review folders.
- **Never touch the editor, its windows or its dialogs.**
- **Never open or read a file inside a foreign drive-root entry** (EZ1). Names, sizes and counts only.
- **You delete nothing.**
- **Never create a junction, symlink or other reparse point** anywhere under `D:\IA_Europa` or the run folder (FA1).
- **The only writes allowed to you:**
  - `code-receipts-094-26b\`;
  - the resume prompt, and the preserved copies of 094-28's report outputs;
  - the two `_reviews\094-26b-*` review files;
  - the ledger row;
  - `D:\IA_Europa\Receipts\CodeReview09426b\`;
  - `D:\IA_Europa\Temp\CodeReview09426b\`.
- **Never print, copy or quote a secret value or the canary value.** Leave account identifiers out of every file you write.
- **P9 v2 applies in full.**
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security, paging, power or machine setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never stop the owner's programs, and never stop the editor: the kit's own limit does that.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
   - **GREEN:** the fixes are built and proven; the smoke session passed; both recipes completed with clean exits; exactly the three declared outputs are new and match the recipe; everything else outside FC5's classes is unchanged; no upload of any kind; the canary is absent; there is no blocking GAP; P9 v2 is unchanged.
   - **NEEDS-DECISION:** the fixes or the smoke failed before any recipe slot; or no idle window; or only EA_Core completed.
   - **RED:** a crash, a protected-state change, or an upload.
2. **Plain summary for the owner** (3 lines).
3. **Start state.**
4. **The Codex job:** run folder, thread, duration, exit, the pauses, the retries, any foreign root entries, and its VERDICT line.
5. **The fixes** in 6–8 lines: FD2, FD3, and the rehearsal.
6. **The run:** the smoke session, then each recipe: gate wait, start, end, duration, exit.
7. **The evidence** in 6–8 lines: clean exits, outputs, authored content, uploads, markers, ledger.
8. **Your review:** the counts, each GAP or OPEN in one line, and your independent results.
9. **Owner actions** for the morning.
10. NEEDS-DECISION items, or "none".
