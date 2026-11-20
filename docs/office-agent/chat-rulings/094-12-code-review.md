# 094-12 — Claude Code review of Codex's Europa A5 finish (W3: written after Codex exited)

**Date:** 2026-10-25, 17:22–17:45 IST · **Reviewer:** Claude Code (mailbox brief 094-12-europa-a5-finish)
**Job:** resume of thread `01a14334` in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`, gpt-6-astra / max / danger-full-access, 2026-10-25 17:11:23 → 17:22:20 IST, **657 s**, exit 0, one try, 23 items, 14 commands.
Codex's verdict: `NEEDS-DECISION | A5 blocked at EA8 entry by the disclosed extra untracked m51 file; no implementation or builds; 110 prior artifacts preserved`.
Codex's report: `_reviews\094-12-codex-report.md` (report.md sha256 `2716E438…`, 7,661 B, collected byte-identical under a one-line header). Ledger row `094-01-europa-phase-a-plan-resume10`, appended once by the relay (I did not add a second).
My receipts: `…\code-receipts-094-12\` (step0, protect, ek1, review, before/after snapshots, canary, launch, poll log, mid-job notes, stray-write note).

## Summary
- **The job stopped at its EA8 entry proof, and the cause is my own mistake.** Before launch, a Bash call of mine failed to enter its folder and its trailing `echo exit=$? >> ek1-prescan-console.txt` wrote a 7-byte file into the session's default directory, which is the protected `m51` checkout (`D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\ek1-prescan-console.txt`, content `exit=1`, sha256 `CF205DBB…2A22`). Its untracked count went 15 → 16. I noticed it, disclosed it (`stray-write-note.txt`, and in the prompt), and left it in place because this brief forbids me to delete anything.
- **Codex stopped correctly.** EA8 (`094-01-ruling.md` lines 186–192) puts "git state (heads, index hashes, status)" in every entry proof, and "On any difference: STOP for chat. Never an automatic restore." My disclosure is context, not a ruling, so it could not lift that rule. Codex changed nothing, restored nothing, and left the file in place.
- **Nothing was harmed:** P9 v2 is 18,264 of 18,264 by my re-hash; the protected state passed before launch and after exit; all 110 bound binaries are equal; 0 engine-tree changes; 0 launches; no commits; refs 72 = 72. D:\IA_Europa gained only 29 new receipt files under `Receipts\09412\`.
- **The canary worked as a positive control and was never leaked:** Codex's base environment saw `IA_CANARY_TOKEN` with the matching hash. Its entry bootstrap dropped it from all 21 git children. My byte search of all 110 files new or changed since launch found the value 0 times.
- **A5 has not moved:** 0 of 6 rebuilds, no denylist wrapper, no module comparison, no makefile proof, no clean-engine query, no rebinding, no docstring removal, no owner kit. EL1's two D2 attempts are unspent.
- **Counts: 16 OK · 8 GAP (all from the entry stop) · 7 OPEN (not exercised) · 1 n/a · 0 STOP violation · 0 CONFLICT.** One observation for chat on EL1's name pattern (O2).

## 1. Rulings
| # | Ruling | Evidence | Status |
|---|---|---|---|
| 1 | EA8 entry proof | STOP at "protected git state": `m51` checkout has `?? ek1-prescan-console.txt` (16 vs 15 untracked); HEAD, index hash and tracked status unchanged (`Receipts\09412\P9\entry\difference.json`). The exit check reproduced the same difference. The full inventory and hash phase did not run, so neither proof is a complete PASS. The cause is my stray write (§2). | OK (correct stop) |
| 2 | EA8 "never an automatic restore" | Codex made no deletion, restore, baseline update or ignore exception. The file is still there (7 B, same hash). | OK |
| 3 | EL1 denylist child environment | Not implemented. The read-only entry bootstrap applied EL1's classes: parent receipt removes 14 names (13 `CLAUDE_*`/`CODEX_*` identifiers + `IA_CANARY_TOKEN`), inherits 84, including `VULKAN_SDK`, `VK_SDK_PATH`, `LINUX_MULTIARCH_ROOT`; 0 tool-prefixed or secret-named names inherited. All 21 git children: `blocked_names` empty, `nonblocked_base_changes` empty, `canary_absent` true (my reading of `entry-git-children.json`, `entry-parent-environment.json`). Routing overrides: `EUROPA_JOB_CONFIG`, `GIT_OPTIONAL_LOCKS`, `PYTHONDONTWRITEBYTECODE`, `TEMP`, `TMP`. No wrapper, UBT or makefile boundary exists yet. **D2 attempts: 0 of 2 used.** | OPEN (not exercised) |
| 4 | EL1 self-tests (SDK pass-through, canary drop) | Not written. | GAP |
| 5 | EL2 canary: generated, name and hash recorded, value only in Code's launch receipt | `canary.txt`: name, UTF-8 and UTF-16LE SHA-256, length 31. The literal value is in no file. `launch-receipt.txt` holds it only XOR-0x5A hex-encoded, so a literal search cannot hit my own receipt. Before launch, a child cmd saw the name, and a child python's hash equalled the generated one. Not in the prompt, notes or this review. | OK |
| 6 | EL2 canary can really fail (positive control) | Codex's parent receipt: `canary_base_present` true, `canary_matches_launch_receipt` true. So the name reached Codex's shell, and EL1's filter had something real to drop. | OK |
| 7 | EL2 canary absent from children and new files | My byte search, all extensions, UTF-8 and UTF-16LE: 110 files new or changed since 17:11:22, 289 MB, across D:\IA_Europa, the run folder, `_reviews`, `C:\ClaudeTemp`, LocalAppData UBT/UE/UHT, ProgramData\Epic, the 4.25 install, the 5.1 engine Intermediate/Saved/Binaries and the m51 checkout. **0 value hits**; 0 errors. The 13 name hits are receipts and scripts that name it (§4). Search self-test 5/5 (`review\canary_search.py`). No makefile exists, so there is no makefile proof yet. | OK (entry scope); post-rebuild OPEN |
| 8 | EL3.2 six rebuilds, probe compiles (incl. vs `d7eee97`) | 0 of 6; no compile. | GAP |
| 9 | EL3.3 / EK2 step 3 post-rebuild makefile proof | 0 makefiles exist (my scans saw 0). My checker `review\mk_check12.py` is written: my reader, blocked names, SDK names, canary bytes, and base-variable comparison by hash. It has not been run, because there is nothing to read. | GAP |
| 10 | EL3.4 final module comparison | Not run. I did not re-run JsonExport: there is no new Codex set to compare against. | GAP |
| 11 | EL3.5 / EI4 clean-engine query | Not run. | GAP |
| 12 | EL3.6 re-bind manifests | Not done. Codex re-hashed the three bound manifests for preservation only. My re-hash: 4.25 27/27, Baseline51 39/39, ReGate51 44/44; manifest hashes `a831dfa0…`, `3d7a8827…`, `797bf35a…` unchanged. The ReGate51 source label still reads `1c4cce7` (094-11 O3). | GAP (rebind) / OK (preserved) |
| 13 | EL3.7 docstrings removed | Not done (worktrees unchanged). | GAP |
| 14 | EL4 owner kit (`.bat` + card) | Neither `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat` nor `_reviews\094-12-owner-card-draft.md` exists. | GAP |
| 15 | EL5 Code's launch-environment deviation | Repeated as accepted: `CLAUDE_CODE_MESSAGING_TOKEN` removed from the wrapper (names only, in the receipt); `IA_CANARY_TOKEN` was then the only secret-named variable, as seen by a child cmd. | OK |
| 16 | EK1 never write or print a secret | Pre-scan (16:50–17:09, 34,860 files, 0 errors, self-test 17/17): 0 secret values; `CLAUDE_CODE_MESSAGING_TOKEN` only as `<redacted>` ×3, `<masked>` ×8, `synthetic`, or a JSON summary key; 0 live-value matches. Post-scan of the 108 new or changed text files with my token and the canary as live values: 0 live matches; the only secret names are source constants, an engine include guard, W4 receipts with no date, and the canary *name* in my own script. | OK |
| 17 | EK3 `GIT_OPTIONAL_LOCKS=0` | In Codex's routing overrides. Every worktree's index hash and mtime is equal before and after in my snapshot. | OK |
| 18 | EJ1 ISPC reporting | No compile. | OPEN |
| 19 | EJ2 do not prune | Worktree porcelain equal before and after; the five prunable registrations intact. | OK |
| 20 | EJ3 CaptureBench host-only | No checker run. | OPEN |
| 21 | EJ5 6 GiB physical for runtime legs | No runtime leg; free physical 8.67–9.69 GiB during the window. | OPEN |
| 22 | EI2 memory gate | No compile. | OPEN |
| 23 | EG1/EI3 engine trees | Equal before/after: 5.1 Binaries\Win64, engine-plugin Binaries, Intermediate\Build, plugin Intermediate\Build, BuildRules, Programs\UnrealBuildTool, UbtRuns, Saved; the full 4.25 install; LocalAppData UE/UBT/UHT; ProgramData\Epic; temp UE folders. No UBT ran. | OK |
| 24 | EG2 build receipts | No build. | OPEN |
| 25 | EG5 P9 v2 | `full-sha256.jsonl` sha256 `8e3ed095…2c74`, 472,552 lines, before and after. My re-hash (seed 9412): 18,244 protected binaries + 20 random = 18,264 OK, 0 mismatches, 0 mtime differences (75 s). | OK |
| 26 | EG5 protected state | PASS at 16:49 and 17:23: release 8/8; six junctions; StackOBot Content = 094-05 snapshot; BankOverflow 0 missing / 0 changed; cache `StackOBot\Binaries` listing `7E8A6C8E…` unchanged. | OK |
| 27 | Stop rule: no launches | 2 polls: 0 build or launch-class processes of ours. 14 commands, all PowerShell, 13 × exit 0 and 1 × exit 1, 0 launch-token mentions (`review\commands-review.json`). | OK |
| 28 | Commits, refs, W4 | No commit, push or tag; refs 72 = 72; `feat/europa-port` `305ecd1`, ReGate51 `89b81a4`, Baseline51 `d7eee97`, Lyra `9781031`, all clean; `m51` `53bf725`, 0 tracked changes. W4 dates absent at launch. | OK |
| 29 | ED1 / ED3 | C 18.26 → 18.98, D 84.06 → 83.80, E 245.66 → 242.72 GiB (the E: drop is outside our trees; D:\IA_Europa grew 224 MB, the ledger). Toolchain reading byte-equal to 094-11's end; Codex's compiler/SDK identity check passed for both engines. | OK |
| 30 | EA11 | Not run: no source change. | n/a |
| 31 | Dated prediction revision | Not needed yet: no authoring leg. | OPEN |
| 32 | Ledger row / report deliverable / 9-hour limit | `…-resume10` once; summary first, sections, `VERDICT:` line; 11 min. | OK |

## 2. The stop, and my part in it
- **What I did:** at 16:49:49 I started the EK1 pre-scan as a background Bash call. Git Bash could not create its temporary cwd, so the `cd` failed and python never ran. The `; echo exit=$? >> ek1-prescan-console.txt` still ran, and it wrote into the session's default directory: the main `m51` checkout. I relaunched the scan through PowerShell, with absolute output paths.
- **When I found it:** at 17:00, in my before-snapshot comparison (`worktree-heads`: 15 → 16 untracked). I disclosed it in `stray-write-note.txt` and in the prompt's Code-added context, and left it in place under "You delete nothing".
- **What I should have done:** treated the changed git status as an entry-proof difference before launching, and ended the run NEEDS-DECISION at that point, rather than spending a Codex launch on a stop I could foresee from EA8's text. The cost: one 11-minute job and one ledger id. No D2 attempt, build or protected state.
- **Lesson** (recorded in my memory notes): never use Bash for anything that writes on this box; give every redirect an absolute path; and before launching a job whose entry proof covers `m51`, compare that checkout's `git status --porcelain` with the recorded count.

## 3. P9 v2 and the protected state, checked independently
- **P9 v2:** row 25.
- **Protected state:** row 26.
- **Snapshots** (61 listings):
  - **094-11 after vs 094-12 before:** 55 equal. The 6 that differ:
    - process list, free space, tooling sizes;
    - the `C:\ClaudeTemp` mtime;
    - D:\IA_Europa: 116 added files, all my own 094-11 review re-runs under `Receipts\CodeReview09411\`, written after that snapshot;
    - the `m51` untracked count (my stray file).
  - **094-12 before vs after:** 56 equal. The 5 that differ:
    - process list, free space, tooling sizes;
    - the `C:\ClaudeTemp` mtime;
    - D:\IA_Europa: exactly 29 added files, all under `Receipts\09412\` (P9 entry/exit, ledger, qualification re-hash, toolchain identity), 0 changed, 0 removed.
  - All three hosts' Binaries, Intermediate, Source, Config, Content and `.uproject`, both plugins' Binaries and Intermediate, the Paris original inventory, worktree heads and indexes, and refs are equal.

## 4. EL2 canary, by name and count
**Name hits** (13 files; every one names the canary and none holds its value):
- **Codex:** `Receipts\09412\entry-parent-environment.json` (the base receipt listing it as removed), `entry09412.py`, `report09412.py`, `events.jsonl`, `last-message.txt`, `report.md`.
- **Mine:** `canary.txt`, `launch-receipt.txt`, three review scripts, `_reviews\094-12-codex-report.md`, and one of my own task-output files in `C:\ClaudeTemp`.

`entry-git-children.json` carries no canary name (`canary_absent` true for all 21 children).

**Not scanned:** the owner's `~/.codex` session store (owner configuration). Codex's rollout there mirrors `events.jsonl`, which has 0 value hits.

**Disclosure.** My first self-test of `canary_search.py` used the real value. For milliseconds it wrote the value into two scratch files in my own `review\` folder, then removed them in the same process. I then changed the self-test to a synthetic stand-in and re-ran it (5/5). No copy remains, and the post-job search above ran afterwards. The next launch generates a new value anyway: the wrapper makes one per run.

## 5. Not exercised, carried forward unchanged from 094-11
EL3 items 1–7, EL4, EK2 step 3, EI4, the probe's first compile on 4.25 (C++17/NoPCHs) and 5.1, and the final descriptor comparison. The 094-11 OK items (EK2 steps 1–2, the reader, the 69 reader can-fails, fixture recipes, runtime readiness and content admission) stand: every worktree is unchanged, so I did not re-run those suites this time.

## 6. Observations for chat
- **O1 (blocking).** The stray file must be removed before any job can pass EA8 against the recorded baseline. The options:
  - (a) the owner deletes `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\ek1-prescan-console.txt`;
  - (b) a ruling lets Code delete it by receipt in the next brief's start checks, with path, size 7 and sha256 `CF205DBB8CEA84897B488ABCC281BF96698D5E94B1096B16657B4CABA9082A22`, then confirm 15 untracked;
  - (c) accept it into the baseline (not recommended: it is junk I made).

  After (a) or (b), 094-12 can be re-issued unchanged. A fresh canary is generated at relaunch.
- **O2 (wording).** EL1 blocks `CLAUDE_*`, and the identifier `CLAUDECODE` (no underscore; an identifier, not a secret) passes that pattern, so Codex's entry bootstrap inherited it. 094-11's checks treated `CLAUDECODE` as an identifier. If chat wants it blocked, EL1's class should name it.
- **O3.** The ReGate51 source label still needs re-binding from `1c4cce7` to `89b81a4` (EL3.6). This is unchanged.
- **O4 (my writes).** These are:
  - `code-receipts-094-12\` and `resume-prompt-094-12.txt`;
  - this file and `_reviews\094-12-codex-report.md`;
  - one line in my own memory notes;
  - the stray file in §2.

  The ledger row was the relay's. I wrote nothing under `D:\IA_Europa`, which is why `CodeReview09412` does not exist: no re-run needed it. I deleted nothing except the two transient self-test files in §4, and I killed nothing.

## 7. Top 5 risks for A6
1. **A5 is still not closed.** The six EI2 rebuilds, the probe's first compiles, the denylist wrapper with its D2 budget, the makefile proof, the clean-engine query and the module comparison are all ahead, and each can stop the job.
2. **EA8 is unforgiving to any write near protected trees, including mine.** Every launcher, Code included, needs the protected git status checked just before launch.
3. **The environment.** The denylist inherits whatever the base environment holds. Today that is 84 names, including the three SDK variables. A future foreign variable (or `CLAUDECODE`, O2) reaches UBT unless it is named, and the module comparison is the only guard for anything UBT reads.
4. **The owner leg is not ready.** No `.bat` and no plain card exist yet. Foreign workloads appear unannounced (this window: Blender at 16:49, two `post.py` render jobs from 17:02), and free physical memory sat at 8.7–9.7 GiB, close to EJ5's 6 GiB.
5. **Content admission is static.** The real 4.25 startup can still surprise; A6's log is the proof.

VERDICT: REVIEW DONE — 16 OK / 8 GAP (EL1 self-tests, EL3.2–3.7 incl. EK2 step 3, EL4 — all from the EA8 entry stop) / 7 OPEN not exercised / 1 n/a / 0 STOP violation / 0 CONFLICT; Codex's EA8 stop is correct and was caused by my 7-byte stray file in the m51 checkout; P9 v2 18,264/18,264, protected state PASS ×2, 110/110 bound binaries, 0 engine-tree changes, 0 launches, 0 commits; canary present in Codex's base environment, dropped from all 21 entry children, 0 value hits in 110 new files; needs chat on O1 (remove the stray file, then re-issue 094-12) and O2 (`CLAUDECODE` and EL1's pattern)
