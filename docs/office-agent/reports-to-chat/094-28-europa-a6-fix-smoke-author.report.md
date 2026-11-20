# REPORT for 094-28-europa-a6-fix-smoke-author.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-28-europa-a6-fix-smoke-author.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-29 21:14:41 | finished: 2026-10-30 03:20:30 | duration: 366 min | exit code: 0
context at last turn: 632106 tokens (63% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-28-europa-a6-fix-smoke-author.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner**
- Codex built and tested the fixes, re-armed the kit, and started the real smoke session once the PC was idle (02:24).
- About 9.5 s in, the kit stopped the editor. A Python guard Codex wrote mistook a normal Unreal SDK check for a build. Neither map was started and nothing in the project changed.
- I also found that a recipe step (turning off anti-aliasing in the post-process settings) can't be done in UE 4.25, so the next smoke will fail on it until chat changes the recipe.

**Start state (21:20 IST, 10-29)**
- `m51` matched P9 (53bf725, index 526aba7f, 0 tracked changes, 15 untracked entries). It also matched before launch, after the job and at 03:20.
- The protected-state check passed. The P9 v2 file is unchanged (8e3ed095…). All 110 of 110 bound files and the 4.25 manifest (c501895c…) matched.
- Feature branch at 97aec94, tools equal to a8d3425, kit still on the 09425 arming. EditorRuns held only 31124.
- My before-lists of the Paris host covered Content (1,453 files, hashed), Config, the descriptor, the `.uproject`, and the Saved, Intermediate and Binaries trees.
- Free space: C: 30.5, D: 44.8, E: 299.1 GiB. Free commit memory 11.9 GiB (DaVinci Resolve still open). Toolchains unchanged.
- The D:/E: launch hold passed at 21:24.

**The Codex job**
- Thread `01a14334`, resumed 21:24:31 → 03:03:49 (20,359 s), exit 0.
- Try 1 hit "Selected model is at capacity"; the relay retried at 22:45 (not a failure).
- No pause was needed: D: dipped to 28.4 GiB (page-file growth), still above the 25 GiB floor. No foreign drive-root changes.
- The real start waited on free commit memory from 00:33 to 02:24.
- Codex's verdict: `NEEDS-DECISION | Smoke stopped on stock UBT SDK validation; both recipes remain unstarted and no packages were created.`
- Its report is collected byte-identical at `_reviews\094-28-codex-report.md`.

**The fixes**
- **FC2:** the launcher now loads the fixture with `imp.load_source` and keeps the module alive. I ran Codex's harness myself under the engine's Python 2.7.14 and it passed (EA_Core 92 calls, EA_Travel 89). My own mutant, the actual 094-26r `author.py` used verbatim, failed with the 094-26r error.
- **FC4:** two switches, `-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False` and `…AnalyticsPrivacySettings]:bSendUsageData=False`. The kit refuses an argv without them. I checked the 4.25 source chain myself:
  - the override is read at ConfigCacheIni.cpp:1151 and applied at :3300;
  - the setting is declared at AnalyticsPrivacySettings.h:18 and read at EditorServer.cpp:1235;
  - EngineAnalytics.cpp:117–183 creates the analytics provider and session-summary writer only when it is true.
  - One citation is imprecise: `-NoAnalytics` is added only on the crash-time launch (WindowsPlatformCrashContext.cpp:840–842). The startup crash monitor's recorded argv has no `-NoAnalytics`.
- **FC5:** the four managed-file classes are in code and match the ruling.
- **FC6:** every UnrealBuildTool child's argv is recorded at each poll, and no build ran. But the guard allows only `-Mode=QueryTargets` (a guess from 094-26r), so it refused the stock, non-build `-Mode=ValidatePlatforms -allplatforms`.
- **Rehearsal:** all three stand-in scenarios passed with 0 Unreal launches. They only tried QueryTargets and Build, so they could not catch this.

**The run**
| Stage | Timing | Result |
|---|---|---|
| Live reading | 00:31 | Only waiting on commit memory |
| Idle pre-check | 00:33 → 02:24:06 | Waited on commit memory, then passed |
| Kit gate | 02:30:00 → 02:30:57 | Passed |
| Smoke editor (PID 2476) | 02:31:55 → 02:32:05 | 9.55 s; kit stop, exit 125; slot quarantined with 0 files |
| EA_Core, EA_Travel | — | Not started |

**The evidence**
- No clean exit was proven: `smoke.json` and `probe.jsonl` were never written, because the log stops during graphics initialisation.
- Runtime.log has 0 error, ensure or fatal lines.
- Every Paris before/after set is equal. `EuropaPhaseA` is absent and no packages exist.
- No upload: 0 send or POST lines; CrashClient.log only names the configured URL. No crash or report folder anywhere.
- EditorRuns still holds only 31124, unchanged.
- Ledger: 0 charged growth on C:, E: and the 5.1 engine; all other rows are foreign.

**My review** (`_reviews\094-28-code-review.md`, 45 rows: 39 OK · 2 N/A · 1 Note · 1 GAP · 1 OPEN · 1 FAIL; P9 state 0 changes)
- **FAIL (G2):** the FC6 allowlist is narrower than the ruling, which stops only build actions. That stopped a healthy session.
- **GAP (G1):** the launcher inside the editor, the native API, a clean exit and the analytics opt-out at shutdown are all still unproven at runtime.
- **OPEN (O1):** 4.25's post-process settings have no anti-aliasing field, but the A5 recipe sets one (`author_425_fixture.py:88–89`). Every other recipe call exists in the 4.25 headers.
- **Observations:**
  - The FC2 fix writes no `.pyc` files only because `PYTHONDONTWRITEBYTECODE` is in the editor's environment (I confirmed the embedded interpreter honours it).
  - The editor rewrites 9 coalesced `.ini` files under Intermediate that FC5 doesn't classify. They are byte-identical so far, so they pass, but a new one would trigger a STOP.
- **My independent checks:**
  - My live compare at 03:15 matched the kit's own reader on 3 polls.
  - Canary: 3,316 files plus 135 decoded `.gz` archives, 0 matches.
  - Command audit: 246 commands, all through the filtered wrapper, matching Codex's count.
  - No account identifiers in any receipt or in the commit.
- **Top 5 risks for 094-27:**
  1. Guards built from guessed stock behaviour (this stop).
  2. Recipe or API steps that fail only in a real session.
  3. The analytics opt-out not yet proven at shutdown, or in a `-game` leg.
  4. Engine-managed writes outside FC5's classes.
  5. Rare idle windows; free commit memory is the usual blocker.

**Owner actions for the morning**
- Nothing is on screen and nothing of ours is running; the PC can be used normally.
- Before the next overnight run, close DaVinci Resolve again and keep the Epic Games Launcher open.
- A tray app (`node server.js --launcher=tray`) grew from 0.8 to 3.8 GB overnight, which is worth restarting.

**NEEDS-DECISION**
1. **FC6:** allow the stock `ValidatePlatforms` child, or switch to a build-only deny-list, then re-arm (the smoke slot is spent).
2. **A5 recipe:** amend the anti-aliasing step for 4.25 before the next smoke, or the smoke will correctly fail again.
3. Whether to require a static check against the 4.25 headers for recipe properties and calls before arming, since the stub harness can't see them.
4. Whether `Intermediate\Config\CoalescedSourceConfigs` joins FC5's managed classes.
api-equivalent cost (not billed on Max): $19.53 | turns: 158 | running total across all briefs: $5,152.89
