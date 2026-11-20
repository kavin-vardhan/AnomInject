# REPORT for 094-26c-europa-a6-recipe-fix-and-author.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-26c-europa-a6-recipe-fix-and-author.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-30 04:33:30 | finished: 2026-10-30 05:49:49 | duration: 76 min | exit code: 0
context at last turn: 617750 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-26c-europa-a6-recipe-fix-and-author.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. Both fixes held and the real smoke passed, but EA_Core's authoring session stopped on a new editor-written file. Its two packages are quarantined and EA_Travel never ran. Codex declared RED; I read it as NEEDS-DECISION (reasons under item 10).

**Plain summary for the owner**
1. The anti-aliasing fix worked: the smoke session found all 51 recipe calls in UE 4.25, and nothing was uploaded.
2. EA_Core built its mesh and map and closed normally, but saving a map makes Unreal write a small bookkeeping file the kit didn't expect, so it set the work aside and stopped.
3. Nothing protected changed and no Unreal window is open; you can use the PC.

**Start state (04:35 IST)**
- m51 matched P9: `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries.
- Protected-state check passed, all 110 bound files matched (4.25 manifest 27 files, `c501895c…`), and the P9 v2 file `8e3ed095…` is unchanged. EditorRuns held only `31124`.
- Feature branch at `c1ef76b`, worktree clean; the kit was still armed for 09426b.
- Free space: D: 72.7 GiB, E: 291.6 GiB, C: 31.0 GiB. Free commit 15.1 GiB, free physical 8.8 GiB.
- Epic launcher open; no Unreal process running.

**The Codex job**
- Run folder `…\runs\2026-10-16-094-01-europa-phase-a-plan`, thread `01a14334`.
- Ran 04:40:26–05:35:47 (3,321 s), exit 0. One relay try, no capacity retry, no pauses, no foreign drive-root entries.
- Codex's verdict: `RED | Smoke passed; EA_Core created a bound WorldState Config file, so both Core packages were quarantined and EA_Travel never started.`

**The fix (commit `9f5ce85`)**
- **FE1:** exactly the two anti-aliasing setters are gone from the recipe and from the smoke checker. The A5 manifest re-binds the recipe (`f55aff4d…`), its document and the checker, and all three hashes match the committed files.
- The dated prediction addendum was committed at 04:53, before the run.
- I ran the Python 2.7 harness myself: Smoke 0, Core 90, Travel 87 calls; both mutants fail; no asset writes.
- **FE2:** only access-denied `.evtx` files directly under `C:\Windows\System32\winevt\Logs` are excluded. My 10 can-fail cases all pass; this run excluded 1 log (GroupPolicy).

**The run**
- **Re-arm:** armed for 09426c with fresh slots and no dry runs; all six older armings refuse. One live, observe-only reading.
- **Smoke:** gate 05:01:06–05:02:04 (cleared on the third poll); editor PID 34352 ran 05:03:05–05:03:35 (29 s), exit 0.
- **EA_Core:** gate 05:10:56–05:11:54; editor PID 43724 ran 05:12:55–05:13:08 (13 s), exit 0. The kit stopped on the Config check and quarantined both packages at 05:13:24–26.
- **EA_Travel:** not started.

**The evidence**
- **Clean exits:** both editors closed normally with one `probe_stop` each and no PIE. The only log error-pattern lines are a Magic Leap notice and "Map check: 0 errors".
- **Why it stopped:** Unreal's `new_level` saves the empty map, then `save_current_level` saves it again. That second save does two things:
  - it writes `Saved\Config\WorldState\1202994220.json` (`EditorActorFolders.cpp:132-149`); 1202994220 is the checksum of the map's path;
  - it copies the earlier empty version of the map to `Saved\Backup\EA_Core_2026-10-30-05-13-05.umap` (`PackageBackup.cpp:101-131`).
- **Outputs:** Content (1,453 files), Config, the descriptor and the `.uproject` are unchanged by hash. The two packages now sit in quarantine, and an empty `Content\EuropaPhaseA` folder remains.
- **Authored content:** the saved map contains all 8 actor labels and the exposure and motion-blur settings, and no anti-aliasing setting. I checked names only, not values.
- **Uploads:** none. Both privacy switches are in both command lines; the crash-client log shows only its configured upload address.
- **Unreal's build tool:** the only child was the stock `-Mode=ValidatePlatforms` check in each session; no compiler, linker or header tool ran.
- **Markers and ledger:** only `31124`, unchanged; C:, E: and 5.1-engine charged growth is 0/0/0.

**My review** (`_reviews\094-26c-code-review.md`, 52 rows)
- **Counts:** 42 OK, 4 N/A, 1 Note, 1 Carried, 1 STOP row, 3 OPEN. Nothing in the fixes failed.
- **OPEN O1:** the WorldState file isn't in any recorded file class, and every map save will write one (EA_Travel's would be `2530609069.json`).
- **OPEN O2:** the `Saved\Backup` copy isn't checked by anything in the kit.
- **OPEN O5:** whether this outcome is RED or NEEDS-DECISION.
- **O3:** the empty `Content\EuropaPhaseA` folder left behind by the quarantine.
- **My own checks:**
  - Live compare equal to the kit's reading.
  - Command audit: 114 commands, 113 through the wrapper, 1 direct read-only, 0 direct writes.
  - Canary scan: 625 files plus 27 decoded archives, 0 matches, 0 errors.
  - Exactly 2 editor launches.
  - EA8 entry and exit proofs passed; m51 matched P9 four times.
  - No push and no tags.

**Owner actions for the morning:** none.

**NEEDS-DECISION (for chat)**
1. **RED or NEEDS-DECISION.** Read by the letter of EY7, no crash, no upload and no protected-state change happened: P9 v2 passed and my protected checks passed. Codex counted the new file in the kit-bound `Saved\Config` as a protected-state difference (EN5:66).
2. **A rule for the WorldState file.** One option is a recorded class limited to the exact name each declared map produces.
3. **A rule for `Saved\Backup`.** Either a recorded class, or a process-only `-ini:EditorPerProjectUserSettings:[FAutoPackageBackup]:Enabled=False`, which would need proving in a smoke first.
4. **The empty folder, and the leftover WorldState and backup files,** in the next arming's before-state.
5. **A fresh re-arm, then the authoring run.** 094-27 stays held until it passes.

Separately: several MCP connectors (Google Calendar and the plugin servers) need authorizing in claude.ai settings or `/mcp`; this job didn't use them.
api-equivalent cost (not billed on Max): $15.89 | turns: 120 | running total across all briefs: $5,183.97
