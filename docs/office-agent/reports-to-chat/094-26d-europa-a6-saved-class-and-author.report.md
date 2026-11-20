# REPORT for 094-26d-europa-a6-saved-class-and-author.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-26d-europa-a6-saved-class-and-author.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-30 05:51:19 | finished: 2026-10-30 07:00:22 | duration: 69 min | exit code: 0
context at last turn: 619636 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-26d-europa-a6-saved-class-and-author.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN. FF2 is built and holds against my own can-fails, and the 4.25 editor authored EA_Core and then EA_Travel, each in one clean session. Exactly the three declared packages are new, nothing was uploaded, no canary leaked, and the protected state is unchanged.

**Plain summary for the owner**
1. Both test maps were built this morning in two short Unreal sessions of about 15 seconds each, while your PC was idle.
2. Unreal's own bookkeeping files under `Saved` are now recorded instead of stopping the run, as chat ruled, and nothing protected changed.
3. Nothing is left running; you can use the PC.

**Start state (05:53 IST)**
- m51 matched P9: `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries.
- The protected-state check passed and all 110 bound files matched (4.25 manifest 27 files, `c501895c…`). The P9 v2 file `8e3ed095…` is unchanged. EditorRuns held only `31124`.
- Feature branch at `aecff71` with a clean worktree. The kit was still armed for 09426c. `Content\EuropaPhaseA` was empty; 094-26c's WorldState file and backup map were still in place.
- Free space: D: 71.6 GiB, E: 291.6, C: 31.0. Free commit 14.9 GiB, free physical 8.7 GiB.
- The Epic launcher was open and no Unreal process was running.

**The Codex job**
- Run folder `…\runs\2026-10-16-094-01-europa-phase-a-plan`, thread `01a14334` (resumed, one try).
- Ran 05:57:20–06:49:16 (3,115 s), exit 0. No pauses, no capacity retry, no foreign drive-root entries.
- Codex's verdict: `GREEN | Both A6 recipes authored fresh with clean exits and three declared packages; FF2 bookkeeping recorded, protection and canary checks passed.`

**The fix (`759a7ba`, then docs commit `1686c4d`)**
- Everything under the host's `Saved\` is now recorded, except a `*.ini` under `Saved\Config\`. The project's `Config\`, the descriptor and the `.uproject` stay protected.
- One config check now runs both at admission and after each session.
- The smoke session is gone (FF3), and the window ends 06:30 IST on 10-31.
- **Codex's suites:** 83 FF cases passed, and the affected FE/FC/EY suites passed on the first attempt.
- **My can-fails:** 17 of 17 behaved as ruled on scratch. A WorldState json, a Backup map, an Autosave, a log and a crash file were recorded; a `Saved\Config` ini, a project `Config` file and the `.uproject` stopped.
- After both sessions, the real host's bound config set still equals the 41 armed rows.

**The run (06:14:01–06:32:44)**

| Recipe | Gate wait | Editor PID | Start–end IST | Duration | Exit |
|---|---|---|---|---|---|
| EA_Core | 06:19:53–06:20:51 (3 quiet polls) | 28984 | 06:21:52–06:22:07 | 15.0 s | 0 |
| EA_Travel | 06:29:20–06:30:18 (3 quiet polls) | 34672 | 06:31:28–06:31:40 | 12.6 s | 0 |

**The evidence**
- **Clean exits:** both sessions ended with a normal `LogExit` and exactly one `probe_stop`, with no PIE world. The log's only error-pattern lines are a Magic Leap notice and "Map check: 0 errors"; there were no ensures.
- **Outputs:** Content went from 1,453 to 1,456 files, and the three new ones are the declared packages: `SM_EA_TwoLod.uasset` (137,553 B, `bbd24a79…`), `EA_Core.umap` (21,114 B, `c2986f9a…`) and `EA_Travel.umap` (21,124 B, `51c7fe5c…`). Config, the descriptor, the `.uproject` and the binaries are unchanged.
- **Saved changes:** two Backup maps and one new WorldState file (`2530609069.json`) were added, and two files were rewritten with no content change. Everything else that changed was asset-registry cache or zero-byte coalesced-config timestamps.
- **Authored content:**
  - Both receipts say AUTHORED_NOT_QUALIFIED, with `lod_count` 2, 8 actor labels and 4 target mappings.
  - Item by item, they match the A5 manifest and the bound recipe.
  - Both maps contain every expected name and no anti-aliasing setting.
  - This checks names and the call sequence only; nobody read back the saved values.
- **Uploads:** none. Both privacy switches are in both command lines (I re-checked their 4.25 source), and the crash-client log shows only its configured upload address.
- **Unreal's build tool:** the only child was the stock `-Mode=ValidatePlatforms -allplatforms` check in each session. No compiler, linker, header tool or shader worker ran.
- **Markers and ledger:** only `31124`, unchanged. Charged growth on C:, E: and the 5.1 engine is 0/0/0. The earlier quarantined EA_Core packages are untouched.

**My review** (`_reviews\094-26d-code-review.md`, 51 rows)
- **Counts:** 40 OK, 5 N/A, 5 Carried, 1 OPEN, no FAIL.
- **G1 (OPEN for chat):** FF2 calls crash-client configs "recorded" but also makes any `Saved\Config` ini a STOP. That config is an ini, and Codex chose STOP. It wasn't exercised: none exists on the host, and `-nowrite` blocks the write. But the engine's two-day purge of old crash-config folders has no such guard.
- **G2 (not blocking here):** under FF2 a new `Saved\Crashes` folder is recorded, and the kit has no separate STOP for it. This run created none.
- **Observations:** O1 Codex's error table omits the Magic Leap line · O2 the Travel editor's command line was unreadable on its last 2 polls as it exited · O3 my launch environment carried `GIT_OPTIONAL_LOCKS=0`, which has no effect · O4 my launch-time estimate was off by 3 minutes · O5 no saved-value readback (A7's job).
- **My own checks:**
  - My before/after lists match exactly: three files added in Content, and nothing else changed outside the recorded classes.
  - My live gate compare matched the kit's reading on 3 polls.
  - **Command audit:** 81 commands, 79 through the wrapper, 2 direct read-only reads, 0 direct writes.
  - **Canary scan:** 666 files plus 28 decoded archives, 0 matches, 0 errors. My planted-value control was found 3 of 3 times.
  - Exactly 2 editor launches.
  - Codex's EA8 entry, entry-fixture and exit proofs passed; m51 matched P9 four times.
  - Protected-state check passed at start and after. No push, no tags.

**Owner actions for the morning:** none.

**NEEDS-DECISION:** nothing blocks this result. For chat:
- **G1:** which reading of FF2 stands for crash-client ini files.
- **G2:** whether 094-27 adds an explicit STOP for new crash folders.

Separately: several connectors (Google Calendar and the plugin servers) need authorizing in claude.ai settings or `/mcp`; this job didn't use them.
api-equivalent cost (not billed on Max): $15.27 | turns: 111 | running total across all briefs: $5,199.24
